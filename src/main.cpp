#include "node_editor.h"
#include "color_node_editor.h"
#include "oscillator.h"
#include "gain.h"
#include "signal_flow_editor.h"

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
#include "imgui-piano.h"

#include "mixer.h"

struct AudioData
{
    std::vector<std::vector<float>> output;
    float sampleRate;
};

void audio_callback(void* user_data, uint8_t* raw_buffer, int bytes)
{
    float* buffer = (float*)raw_buffer;
    int nChans = 2;
    int sampsPerChan = bytes / sizeof(float) / nChans;
    AudioData* data = (AudioData*)user_data;

    // output is interleaved LRLRLR
    for (int i = 0; i < sampsPerChan; i++) {
        for (size_t ch = 0; ch < nChans; ch++) {
            buffer[(i * nChans) + ch] = (float)data->output[ch][i];
        }
    }
}

void testDevices() {
    for (uint8_t i = 0; i < SDL_GetNumAudioDrivers(); ++i) {
        printf("Audio driver %d: %s\n", i, SDL_GetAudioDriver(i));
    }

    std::cout << "current audio driver is " << SDL_GetCurrentAudioDriver() << std::endl;

    int nbDevice = SDL_GetNumAudioDevices(0);

    for (int i = 0; i < nbDevice; ++i) {
        std::cout << "device num" << i << ": ";
        std::cout << SDL_GetAudioDeviceName(i, 0) << std::endl;
    }
}

const char* initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
    }

    // YOU WILL NOT GET SOUND WITHOUT THIS ENV VAR
    _putenv("SDL_AUDIODRIVER=DirectSound");

    SDL_Init(SDL_INIT_AUDIO);
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

bool pianoCallback(void* UserData, int Msg, int Key, float Vel) {
    if (Key >= 128) return false; // midi max keys
    /*if (Msg == NoteGetStatus) return KeyPresed[Key];
    if (Msg == NoteOn) { KeyPresed[Key] = true; Send_Midi_NoteOn(Key, Vel * 127); }
    if (Msg == NoteOff) { KeyPresed[Key] = false; Send_Midi_NoteOff(Key, Vel * 127); }*/
    return true;
}

int main(int, char**)
{

    const char* glsl_version = initSDL();

    SDL_WindowFlags window_flags =
        (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    auto window = SDL_CreateWindow(
        "Dear ImGui SDL2+OpenGL3 example",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        720,
        window_flags);
    auto glContext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glContext);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto io = ImGui::GetIO();

    ImNodes::CreateContext();
    SignalFlowEditor signalFlowEditor;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();
    ImNodes::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Set up audio output
    size_t bufSize = 2048;
    AudioData audioData; // poll the editor if there is an output node to fill the buffer in audioData
    std::vector<float> left(bufSize);
    std::vector<float> right(bufSize);
    audioData.output.push_back(left);
    audioData.output.push_back(right);
    SDL_AudioSpec want;
    size_t sample_nr = 0;
    want.freq = 44100; // number of samples per second
    want.format = AUDIO_F32; // sample type (here: signed short i.e. 16 bit)
    want.channels = 2;
    want.samples = bufSize; // buffer-size
    want.callback = audio_callback; // function SDL calls periodically to refill the buffer
    want.userdata = &audioData; // counter, keeping track of current sample number

    SDL_AudioSpec have;
    SDL_AudioDeviceID device;
    testDevices();
    auto num = SDL_GetNumAudioDevices(0);
	device = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(1, 0), 0, &want, &have, 0);
	SDL_PauseAudioDevice(device, 0);

   ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;

    while (!done)
    {

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        static int PrevNoteActive = -1;
        ImGui_PianoKeyboard("PianoTest", ImVec2(1024, 100), &PrevNoteActive, 21, 108, pianoCallback, nullptr, nullptr);

        ImGui::ShowDemoWindow(nullptr);
        signalFlowEditor.show();

        //NodeEditor::NodeEditorShow();
        // edit = NodeEditor::NodeEditorAccess();

        // Rendering
        ImGui::Render();
        auto& io = ImGui::GetIO();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(
            clear_color.x * clear_color.w,
            clear_color.y * clear_color.w,
            clear_color.z * clear_color.w,
            clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImNodes::DestroyContext();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_CloseAudio();
    SDL_Quit();

    return 0;
}
