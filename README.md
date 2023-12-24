# Synthcastle
...building castles made of sound...

<img src="https://github.com/aparks5/synthcastle/blob/main/synthcastle-readme.gif" width="450">

## Description

Synthcastle is a node-based sandbox for audio algorithms and sound design, like
Max/PD but with higher level abstractions, closer to a modular synthesizer.
I have only built this on Windows 10, compiled in Visual Studio. I would
greatly appreciate any PRs to get this building on alternate platforms, as I do
not own a mac and all my linux boxes are servers without displays.
The chosen libs and technologies should help make this easier to port to other platforms.
Synthcastle uses cmake, ImGui, PortAudio, RtMidi, and OpenGL (GLAD + GLFW).

## Features
- At the heart of synthcastle are the `nodes`, which are DSP processing
components that form the building blocks of each castle.
- Freely assignable N-input + N-output per node graph-based architecture for
flexible sound processing.
- Simple interface with as little menu/dropdown diving as possible, it should
be nearly totally transparent as to the values of each node's parameters.
- Supports many processing blocks like oscillators, LPF, delay, distortion,
mixing, panning, looping. Most nodes support modulation capabilities.
- Features a 4 voice sequencer with 128 session-storable patterns, many
different scales plus randomness possibilities with the Probability knob.
- Graph-based architecture should make it fairly straightforward to add new
nodes.

## News 
### 09/2023
- `imgui` UI overhaul has been merged to main
### 08/2023
- Complete UI overhaul in progress using imgui, implot, imnodes
- `imgui` branch now open for business
### 12/2021
- Synthcastle text-based interface supports 8 voices, pattern command language, drum sample playback, LFOs, LPF, ADSR, reverb, delay. 
### 08/2021
- Synthcastle is born.



## Excellent reference books used to help make this:
- Will Pirkle - Designing Software Synthesizer Plugins in C++ 
- Will Pirkle - Designing Audio Effect Plugins in C++
- Andrist and Sehr - C++ High Performance
- Scott Meyers - Effective Modern C++
- Anthony Williams - C++ Concurrency in Action
- https://learnopengl.com/Getting-started/Hello-Window
