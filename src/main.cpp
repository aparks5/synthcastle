
#include <thread>

#define BLOCK_SIZE (4096)


#include "model.h"
#include "view.h"
#include "controller.h"

#include "audioinput.h"
#include "relay.h"
#include "value.h"
#include "node_editor.h"
#include "signal_flow_editor.h"
#include "util.h"
#include "portaudio.h"
#include "portaudiohandler.h"
#include "pa_win_wasapi.h"
#include "windows.h"

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

#define MAX_NODES (2048)

struct AudioData
{
    const PaStream* stream;
    int idVisited[MAX_NODES];
    std::shared_ptr<Controller> controller;
    std::shared_ptr<moodycamel::ReaderWriterQueue<Commands>> commandsToAudioCallback;
    std::shared_ptr<moodycamel::ReaderWriterQueue<Commands>> commandsFromAudioCallback;
    std::shared_ptr<IOServer> server;
    std::vector<std::vector<float>> callbackBuffer;
    float left;
    float right;
};

static void evaluate(float inputSample, std::shared_ptr<NodeGraph> graph, std::shared_ptr<std::stack<int>> postorder, float& left, float& right) noexcept
{
	left = 0;
	right = 0;

	if ((!graph.get()) || (!postorder.get())) {
		return;
	}


	if (graph->getRoot() == -1) {
		return;
	}

    if (postorder->size() == 0) {
        return;
    }


    // we need to make a copy because we're about to pop off
    std::stack<int> traversal = *postorder;

	std::stack<float> value_stack;
	int idVisited[1024];
	memset(idVisited, 0, sizeof(idVisited));

	std::array<float, 16> cached{};
	int cacheIdx = 0;
	std::fill(cached.begin(), cached.end(), 0);

	while (!traversal.empty())
	{
		const int id = traversal.top();
		idVisited[id]++;
		traversal.pop();
		const auto& pNode = graph->node(id);

		if (pNode->getName() == "audio_input") {
			pNode->params[AudioInput::ADC_INPUT] = inputSample;
		}

		switch (pNode->getNodeType()) {
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

			for (int idx = 0; idx < 16; idx++) {
				cached[idx] = 0;
			}

			cacheIdx = 0;
			for (auto& out : pNode->outputs) {
				cached[cacheIdx++] = out;
				if (cacheIdx >= cached.size()) {
					break;
				}
			}

			// value_stack should have two items from the output node.
			// output is connected to nothing so these will not show up as PROCESSOR_OUTPUTs
			// in the graph
			if (pNode->getName() == "output") {
				left = cached[0];
				right = cached[1];
				return;
			}

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
		default:
			break;
		}
	}
}

static int paCallbackMethod(const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData)
{
    AudioData* data = (AudioData*)userData;

    int* idVisited = data->idVisited;
    float* out = (float*)outputBuffer;
    float* in = (float*)inputBuffer;

    auto nodeGraph = std::atomic_load(&(data->controller->m_graph));
    auto postorder = std::atomic_load(&(data->controller->m_traversal));
    
    (void)timeInfo; /* Prevent unused variable warnings. */
    (void)statusFlags;
    (void)inputBuffer;

    for (size_t sampIdx = 0; sampIdx < framesPerBuffer; sampIdx++) {
        float output = 0.f;
        float inputSample = *in++;
        in++; // skip other channel
        evaluate(inputSample, nodeGraph, postorder, data->left, data->right);
            // write interleaved output -- L/R
        *out++ = data->left;
        *out++ = data->right;
        data->callbackBuffer[0][sampIdx] = data->left;
        data->callbackBuffer[1][sampIdx] = data->right;
    }

    // perform all queued updates to modify the graph safely
    // this should all happen on another thread
    // use a SPSC queue to tell the controller to update
    // this should never be resized except at init time to accommodate user driven buffer size
    data->controller->sendBuffer(data->callbackBuffer);

    /*auto res = Pa_GetStreamCpuLoad(data->stream);
    if (res > 0.9f) {
        std::cout << "cpu usage exceeds 90% on this thread" << std::endl;
    }*/

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
    audioData->left = 0;
    audioData->right = 0;

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
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultHighOutputLatency;
    PaWasapiStreamInfo wasapiInfo;
    wasapiInfo.hostApiType = paWASAPI;
    wasapiInfo.version = 1;
    wasapiInfo.flags = 0;
    wasapiInfo.flags |= paWinWasapiExclusive;
    outputParameters.hostApiSpecificStreamInfo = &wasapiInfo;

    PaStreamParameters inputParameters;
    
    int hostNr = Pa_GetHostApiCount();

    std::vector<const PaHostApiInfo*> infoVertex;
    for (int t = 0; t < hostNr; ++t) {
        infoVertex.push_back(Pa_GetHostApiInfo(t));
    }

    int inputIndex = Pa_GetDefaultInputDevice();
    inputParameters.device = inputIndex;

    // look for WASAPI device if we can find one
    for (auto& item : infoVertex) {
        printf("host: %s\n", item->name);
        if (item->type == paWASAPI) {
            inputIndex = item->defaultInputDevice;
            index = item->defaultOutputDevice;
            inputParameters.device = inputIndex;
            outputParameters.device = index;
        }
    }

    inputParameters.channelCount = 2;       // mono input
    inputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    PaError err = Pa_OpenStream(&stream, &inputParameters,
        &outputParameters, 44100, BLOCK_SIZE, paClipOff, &paCallbackMethod, audioData.get());

    if (err != paNoError) {
        printf("Failed to open stream to device !!!");
    }
    err = Pa_StartStream(stream);
    audioData->stream = stream;
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

