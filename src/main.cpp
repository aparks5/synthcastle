#include "model.h"
#include "view.h"
#include "controller.h"

#include "node_editor.h"
#include "oscillator.h"
#include "fourvoice.h"
#include "filter.h"
#include "gain.h"
#include "signal_flow_editor.h"
#include "output.h"
#include "util.h"
#include "portaudio.h"
#include "portaudiohandler.h"
#include "windows.h"
#include "quadmixer.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include "imnodes.h"
#include <SDL2/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL2/SDL_opengles2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif

#include <stdio.h>
#include <iostream>
#include "mixer.h"
#include <mutex>

struct AudioData
{
    std::vector<std::vector<float>> output;
    float sampleRate;
    // const NodeGraph* graph;
};

std::tuple<float,float> evaluate(const NodeGraph& graph)
{
    int root_node = graph.m_root;
    if (root_node == -1) {
        return std::make_tuple(0.,0.);
    }
	std::stack<int> postorder;
	dfs_traverse(graph, root_node, [&postorder](const int node_id) -> void { postorder.push(node_id); });

    int voiceCount = 0;

	std::stack<float> value_stack;
	while (!postorder.empty())
	{
		const int id = postorder.top();
		postorder.pop();
		const auto& pNode = graph.node(id);

		switch (pNode->type) {
        case NodeType::FOUR_VOICE:
        {
            auto in = value_stack.top();
            value_stack.pop();
            // MIDI block should push all the voices
            // to the stack
            value_stack.push(in);
		}
        break;
        case NodeType::FILTER:
        {
            auto in = value_stack.top();
            value_stack.pop();
            auto mod = value_stack.top();
            value_stack.pop();
            auto depth = value_stack.top();
            value_stack.pop();

            pNode->params[Filter::FREQMOD] = mod;
            pNode->params[Filter::MODDEPTH] = depth;
            //printf("depth %f, \t mod %f, \t in %f\n", depth, mod, in);
            value_stack.push(pNode->process(in));
        }
        break;
        case NodeType::QUAD_MIXER:
        {
            auto d = value_stack.top();
            value_stack.pop();
            auto c = value_stack.top();
            value_stack.pop();
            auto b = value_stack.top();
            value_stack.pop();
            auto a = value_stack.top();
            value_stack.pop();
            pNode->params[QuadMixer::INPUT_A] = a;
            pNode->params[QuadMixer::INPUT_B] = b;
            pNode->params[QuadMixer::INPUT_C] = c;
            pNode->params[QuadMixer::INPUT_D] = d;
            value_stack.push(pNode->process());
        }
        break;
        case NodeType::MIDI_IN:
        {
            // push all voices to stack
            value_stack.push(pNode->process());
        }
        break;
		case NodeType::OSCILLATOR:
		{
			pNode->update();
			auto freq = value_stack.top();
			value_stack.pop();
			if (freq != INVALID_PARAM_VALUE) {
                // scale midi as float to hz
                float target = 0;
                if ((freq * 128) > 15) {
                    target = midiNoteToFreq((int)(freq * 128.));
                }
                pNode->params[Oscillator::FREQ] = target;
			}
			auto mod = value_stack.top();
			value_stack.pop();
			if (mod != INVALID_PARAM_VALUE) {
				pNode->params[Oscillator::MODFREQ] = mod;
			}
			auto depth = value_stack.top();
			value_stack.pop();
			if (depth != INVALID_PARAM_VALUE) {
				pNode->params[Oscillator::MODDEPTH] = depth;
			}

			auto temp = pNode->process();
			value_stack.push(temp);
		}
		break;
        case NodeType::CONSTANT:
        {
            auto val = pNode->process();
            value_stack.push(val);
        }
        break;
		case NodeType::GAIN:
		{
			auto in = value_stack.top();
			value_stack.pop();
			auto mod = value_stack.top();
			value_stack.pop();
			pNode->params[Gain::GAINMOD] = abs(mod);
			auto val = pNode->process(in);
			value_stack.push(val);
		}
		break;
		case NodeType::VALUE:
		{
             //if the edge does not have an edge connecting to another node, then just use the value
            // at this node. it means the node's input pin has not been connected to anything and
            // the value comes from the node's ui.
			if (graph.num_edges_from_node(id) == 0ull) {
				value_stack.push(pNode->value);
			}
		}
        break;
        case NodeType::OUTPUT:
        {
			// The final output node isn't evaluated in the loop
			if (!value_stack.empty()) {
				float left = value_stack.top();
				value_stack.pop(); // stack should be empty now
				pNode->params[Output::DISPLAY_L] = left;
                if (value_stack.empty()) {
                    return std::make_tuple(left, 0.);
                }

                if (!value_stack.empty()) {
                    float right = value_stack.top();
                    pNode->params[Output::DISPLAY_R] = right;
                    value_stack.pop(); // stack should be empty now
					return std::make_tuple(left, right);
                }
			}
		}
		break;
		default:
			break;
		}
	}

    return std::make_tuple(0.,0.);
}

static int paCallbackMethod(const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData)
{
    AudioData* data = (AudioData*)userData;

    auto* out = (float*)outputBuffer;

    (void)timeInfo; /* Prevent unused variable warnings. */
    (void)statusFlags;
    (void)inputBuffer;

    for (size_t sampIdx = 0; sampIdx < framesPerBuffer; sampIdx++) {
        float output = 0.f;
        auto lr = std::pair{0, 0}; // eventually: controller->evaluate());
            // write interleaved output -- L/R
		*out++ = std::get<0>(lr);
		*out++ = std::get<1>(lr);
    }

    return paContinue;
}

static void paStreamFinishedMethod()
{
    printf("Finished playback.");
    return;
}

const char* initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    return glsl_version;
}

int main(int, char**)
{

	// TODO: MOVE to portaudio class
    {
        PaStream* stream;
        PortAudioHandler paInit;

        // Set up audio output
        size_t bufSize = 8192;
        AudioData audioData; // poll the editor if there is an output node to fill the buffer in audioData
        // audioData.graph = signalFlowEditor.graph();

        PaStreamParameters outputParameters;

        int index = Pa_GetDefaultOutputDevice();
        outputParameters.device = index;
        if (outputParameters.device == paNoDevice) {
            return false;
        }

        const PaDeviceInfo* pInfo = Pa_GetDeviceInfo(index);
        if (pInfo != 0)
        {
            printf("Output device name: '%s'\r", pInfo->name);
        }

        outputParameters.channelCount = 2;       /* stereo output */
        outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
        outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
        outputParameters.hostApiSpecificStreamInfo = NULL;

        PaError err = Pa_OpenStream(
            &stream,
            NULL, /* no input */
            &outputParameters,
            44100,
            1024,
            paClipOff,      /* we won't output out of range samples so don't bother clipping them */
            &paCallbackMethod,
            &audioData            /* Using 'this' for userData so we can cast to MixerStream* in paCallback method */
        );

        if (err != paNoError) {
            printf("Failed to open stream to device !!!");
        }
        err = Pa_StartStream(stream);
        if (err != paNoError) {
            printf("Failed to start stream!!!");
        }
        if (err != paNoError) {
            Pa_CloseStream(stream);
            stream = 0;
        }
    }

    auto model = std::make_shared<Model>();
    auto view = std::make_shared<View>();
    auto controller = std::make_shared<Controller>(view, model);
    view->addListener(controller);
    view->run();

    return 0;
}
