
#include <thread>

#define BLOCK_SIZE (1024)

#include "model.h"
#include "view.h"
#include "controller.h"

#include "seq.h"
#include "constant.h"
#include "sampler.h"
#include "trig.h"
#include "oscillator.h"
#include "delay.h"
#include "relay.h"
#include "value.h"

#include "node_editor.h"
#include "oscillator.h"
#include "fourvoice.h"
#include "filter.h"
#include "freqshift.h"
#include "gain.h"
#include "midi.h"
#include "signal_flow_editor.h"
#include "output.h"
#include "util.h"
#include "portaudio.h"
#include "portaudiohandler.h"
#include "windows.h"
#include "quadmixer.h"

#include "commands.h"

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
    std::shared_ptr<Controller> controller;
	std::shared_ptr<moodycamel::ReaderWriterQueue<Commands>> commandsToAudioCallback;
	std::shared_ptr<moodycamel::ReaderWriterQueue<Commands>> commandsFromAudioCallback;
	std::shared_ptr<IOServer> server;
	std::vector<std::vector<float>> callbackBuffer;

};

static std::tuple<float,float> evaluate(float inputSample, std::shared_ptr<NodeGraph> graph) noexcept
{

    if (!graph.get()) {
        return { 0,0 };
    }

	float clockCache = -1.f;

	if (graph->getRoot() == -1) {
		return {0, 0};
	}

	std::stack<int> postorder;
	//dfs_traverse(graph, [&postorder](const int node_id) -> void { postorder.push(node_id); });

	std::stack<float> value_stack;
    // traverse all nodes and say none of them have been visited yet
    // reset FLAGS before traversal
    // i don't want to have to do two traversals
	//std::stack<int> postorder;
	dfs_traverse(graph, [&postorder](const int node_id) -> void { postorder.push(node_id); });

    size_t nodeCount = postorder.size();

    // make a hashmap of ids and count visited
    std::unordered_map<int, int> idVisited;
// check stack size?    
    std::vector<float> cached;
    cached.reserve(16);
	while (!postorder.empty())
	{
		const int id = postorder.top();

        idVisited[id]++;
		postorder.pop();
		const auto& pNode = graph->node(id);

		switch (pNode->getNodeType()) {
		case NodeType::AUDIO_IN:
		{
            value_stack.push(inputSample);
		}
		break;
		case NodeType::DELAY:
		{
			auto val = value_stack.top();
			value_stack.pop();
			value_stack.push(pNode->process(val));
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
            value_stack.push(pNode->process(in));
        }
        break;
        case NodeType::FREQ_SHIFT:
        {
            auto in = value_stack.top();
            value_stack.pop();
            pNode->params[FrequencyShifter::INPUT] = in;
            auto modfreq = value_stack.top();
            value_stack.pop();
            pNode->params[FrequencyShifter::MODFREQ] = modfreq;

            if (idVisited[id] == 1) {
                cached.clear();
                pNode->process();
                cached.push_back(pNode->params[FrequencyShifter::OUTPUT]);
            }
        }
        break;
        case NodeType::ENVELOPE:
        {
			// pop off the stack in input order
            auto in = value_stack.top();
            value_stack.pop();
            auto trig = value_stack.top();
            value_stack.pop();
            pNode->params[Envelope::TRIG] = trig;

            if (idVisited[id] == 1) {
                pNode->process(in);
            }

            cached.clear();
            cached.push_back(pNode->params[Envelope::OUTPUT]);
        }
        break;
        case NodeType::LOOPER:
        {
            auto val = value_stack.top();
            value_stack.pop();
            value_stack.push(pNode->process(val));
        }
        break;
        case NodeType::MIDI_IN:
        {
			if (idVisited[id] == 1) {
                cached.clear();
				pNode->process();
                cached.push_back(pNode->params[MIDI::OUT_VOICE1]);
                cached.push_back(pNode->params[MIDI::OUT_VOICE2]);
                cached.push_back(pNode->params[MIDI::OUT_VOICE3]);
                cached.push_back(pNode->params[MIDI::OUT_VOICE4]);
                cached.push_back(pNode->params[MIDI::VELOCITY]);
                break;
			}
        }
        break;
        case NodeType::PROCESSOR:
        {
			for (size_t idx = 0; idx < pNode->inputs.size(); idx++) {
                auto val = value_stack.top();
                pNode->inputs[idx] = val;
                value_stack.pop();
            }

            if (idVisited[id] == 1) {
                pNode->process();
            }

            cached.clear();
            for (auto& out : pNode->outputs) {
                cached.push_back(out);
            }
        }
        break;
        case NodeType::SEQ:
        {
			// i should queue this til after eval
			// pop off the stack in input order
            auto trig = value_stack.top();
            value_stack.pop();
            pNode->params[Seq::TRIGIN] = trig;
            auto reset = value_stack.top();
            value_stack.pop();
            pNode->params[Seq::RESET] = reset;

            if (idVisited[id] == 1) {
                pNode->process();
			}

            // allow one out to branch to multiple ins
            // repopulate the cache each visit
			cached.clear();
			auto val = pNode->params[Seq::TRIGOUT];
			cached.push_back(val);

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
		case NodeType::SAMPLER:
		{
			pNode->update();

			auto val = value_stack.top();
			value_stack.pop();
			pNode->params[Sampler::PITCH] = val;

			auto pos = value_stack.top();
			value_stack.pop();
			pNode->params[Sampler::POSITION] = pos;

			auto startstop = value_stack.top();
			value_stack.pop();
			pNode->params[Sampler::STARTSTOP] = startstop;

			value_stack.push(pNode->process());
		}
		break;
		case NodeType::OSCILLATOR:
		{
			pNode->update();
			auto freq = value_stack.top();
			value_stack.pop();
            if (freq > 0) {
                pNode->params[Oscillator::FREQ] = freq;
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

            if (idVisited[id] == 1) {
                pNode->process();
            }

            cached.clear();
            cached.push_back(pNode->params[Oscillator::OUTPUT]);
		}
		break;
        case NodeType::PROCESSOR_OUTPUT:
        {
            // if there is a Relay it's attached to a process node
            // push its index to the stack to obtain the output?
            // just pull the value-th item from the cache
            value_stack.push(cached[pNode->outputs[ProcessorOutput::OUTPUT]]);
        }
        break;
		case NodeType::DISTORT:
		{
			auto in = value_stack.top();
			value_stack.pop();
			auto val = pNode->process(in);
			value_stack.push(val);
		}
		break;
        case NodeType::TRIG:
        {
			if (idVisited[id] == 1) {
				pNode->process();
			}

			cached.clear();
			auto val = pNode->params[Trig::TRIGOUT];
			cached.push_back(val);
        }
        break;
		case NodeType::GAIN:
		{
			auto in = value_stack.top();
			value_stack.pop();
			auto mod = value_stack.top();
			value_stack.pop();

			auto panmod = value_stack.top();
			value_stack.pop();

			if (idVisited[id] == 1) {
                cached.clear();
				pNode->params[Gain::INPUT] = in;
				pNode->params[Gain::GAINMOD] = abs(mod);
				pNode->params[Gain::PANMOD] = abs(panmod);
				pNode->process();
                cached.push_back(pNode->params[Gain::LEFTOUT]);
                cached.push_back(pNode->params[Gain::RIGHTOUT]);
                break;
			}
		}
		break;
		case NodeType::PROCESSOR_INPUT:
		{
             //if the edge does not have an edge connecting to another node, then just use the value
            // at this node. it means the node's input pin has not been connected to anything and
            // the value comes from the node's ui.
			if (graph->num_edges_from_node(id) == 0ull) {
				value_stack.push(pNode->outputs[ProcessorInput::OUTPUT]);
			}
		}
        break;
        case NodeType::OUTPUT:
        {
            if (pNode->params[Output::MUTE] == 1) {
				return std::make_tuple(0., 0.);
            }
			if (!value_stack.empty()) {
				float left = value_stack.top();
				value_stack.pop(); 
				pNode->params[Output::DISPLAY_L] = left;
                if (value_stack.empty()) {
                    return std::make_tuple(left, 0.);
                }
				else {
                    float right = value_stack.top();
                    pNode->params[Output::DISPLAY_R] = right;
                    value_stack.pop(); 
					return std::make_tuple(left, right);
                }
            }
            else {
                return std::make_tuple(0., 0.);
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

    float* out = (float*)outputBuffer;
    float* in = (float*)inputBuffer;

    auto nodeGraph = std::atomic_load(&(data->controller->m_graph));
    
    (void)timeInfo; /* Prevent unused variable warnings. */
    (void)statusFlags;
    (void)inputBuffer;

    for (size_t sampIdx = 0; sampIdx < framesPerBuffer; sampIdx++) {
        float output = 0.f;
        float inputSample = *in++;
        in++; // skip other channel
        std::tuple<float, float> lr = evaluate(inputSample, nodeGraph);
            // write interleaved output -- L/R
		auto l = std::get<0>(lr);
		auto r = std::get<1>(lr);
        *out++ = l;
        *out++ = r;
        data->callbackBuffer[0][sampIdx] = l;
        data->callbackBuffer[1][sampIdx] = l;
    }

    // perform all queued updates to modify the graph safely
    // this should all happen on another thread
    // use a SPSC queue to tell the controller to update
    // this should never be resized except at init time to accommodate user driven buffer size
    data->controller->sendBuffer(data->callbackBuffer);

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

void runGui(std::shared_ptr<AudioData> data)
{
    auto model = std::make_shared<Model>();
    auto controller = std::make_shared<Controller>(model);
    data->controller = controller;

    auto view = std::make_shared<View>();
    view->addListener(controller);
	view->run();
}

int main(int, char**)
{
    std::shared_ptr<AudioData> audioData = std::make_shared<AudioData>();
    std::vector<float> myRow(BLOCK_SIZE, 0.f);
    for (size_t idx = 0; idx < 2; idx++) {
        audioData->callbackBuffer.push_back(myRow);
    }

	PaStream* stream;
	PortAudioHandler paInit;
	size_t bufSize = BLOCK_SIZE * sizeof(float);
	PaStreamParameters outputParameters;

	int index = Pa_GetDefaultOutputDevice();
	outputParameters.device = index;
	if (outputParameters.device == paNoDevice) {
		return false;
	}
	const PaDeviceInfo* pInfo = Pa_GetDeviceInfo(index);
	if (pInfo != 0) {
		printf("Output device name: '%s'\r", pInfo->name);
	}
	outputParameters.channelCount = 2;       /* stereo output */
	outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	PaStreamParameters inputParameters;
    
    int hostNr = Pa_GetHostApiCount();

    std::vector<const PaHostApiInfo*> infoVertex;
    for (int t = 0; t < hostNr; ++t) {
        infoVertex.push_back(Pa_GetHostApiInfo(t));
    }

    int inputIndex = Pa_GetDefaultInputDevice();
    inputParameters.device = inputIndex;

  //  // look for WASAPI device if we can find one
  //  for (auto& item : infoVertex) {
  //      printf("host: %s\n", item->name);
		//if (item->type == paWASAPI) {
		//	inputIndex = item->defaultInputDevice;
		//	index = item->defaultOutputDevice;
		//	inputParameters.device = inputIndex;
		//	outputParameters.device = index;
  //      }
  //  }

	inputParameters.channelCount = 2;       // mono input
	inputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;

	PaError err = Pa_OpenStream(&stream, &inputParameters,
		&outputParameters, 44100, BLOCK_SIZE, paClipOff, &paCallbackMethod, audioData.get());

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

	std::thread gui(runGui, audioData);
    gui.join();

	return 0;
}

