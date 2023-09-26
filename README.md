Welcome to...
```
  _______     ___   _ _______ _    _  _____           _____ _______ _      ______ 
 / ____\ \   / / \ | |__   __| |  | |/ ____|   /\    / ____|__   __| |    |  ____|
| (___  \ \_/ /|  \| |  | |  | |__| | |       /  \  | (___    | |  | |    | |__   
 \___ \  \   / | . ` |  | |  |  __  | |      / /\ \  \___ \   | |  | |    |  __|  
 ____) |  | |  | |\  |  | |  | |  | | |____ / ____ \ ____) |  | |  | |____| |____ 
|_____/   |_|  |_| \_|  |_|  |_|  |_|\_____/_/    \_\_____/   |_|  |______|______|

                                  |>>>
                                  |
                    |>>>      _  _|_  _         |>>>
                    |        |;| |;| |;|        |
                _  _|_  _    \\.    .  /    _  _|_  _
               |;|_|;|_|;|    \\:. ,  /    |;|_|;|_|;|
               \\..      /    ||;   . |    \\.    .  /
                \\.  ,  /     ||:  .  |     \\:  .  /
                 ||:   |_   _ ||_ . _ | _   _||:   |
                 ||:  .|||_|;|_|;|_|;|_|;|_|;||:.  |
                 ||:   ||.    .     .      . ||:  .|
                 ||: . || .     . .   .  ,   ||:   |       \,/
                 ||:   ||:  ,  _______   .   ||: , |            /`\
                 ||:   || .   /+++++++\    . ||:   |
                 ||:   ||.    |+++++++| .    ||: . |
              __ ||: . ||: ,  |+++++++|.  . _||_   |
     ____--`~    '--~~__|.    |+++++__|----~    ~`---,              ___
-~--~                   ~---__|,--~'                  ~~----_____-~'   `~----~~                                                                                  
```

# News 
## 09/2023
- `imgui` UI overhaul has been merged to main
## 08/2023
- complete UI overhaul in progress using imgui, implot, imnodes
- `imgui` branch now open for business

# synthcastle
an interactive text-based synthesizer in modern c++

like building sandcastles in sound





### Description

This project aims to create a musical instrument and prototyping ground for new audio algorithms.

I have only built this on Windows 10, compiled in Visual Studio.

It uses cmake, PortAudio, RtMidi, and OpenGL (GLAD + GLFW).

The syntax once you run `main.cpp` is very simple:

```
PortAudio Test: output triangle wave. SR = 44100, BufSize = 256
Output device name: 'Speakers (Elektron Digitakt)'

>>> commands: stop, osc, freq, filt-freq, filt-q, filt-lfo-freq, pitch/filt-lfo-on/off,
>>> pitch-lfo-freq, pitch-lfo-depth, reverb-on/off, chorus-on/off, delay-on/off
>>> osc2-enable, osc2-coarse, osc2-fine, env [attackMs decayMs susdB]
>>> play [note], loop [note-note2-note3:duration,note4:duration2... loopNumTimes]
>>>
```

The OpenGL visualizer window currently has three modes. You must press 1, 2, 3 to switch modes:
```
1 - Time-domain oscilloscope (256 sample display)
2 - VU RMS meter
3 - FFT (256 samples, real-valued power spectrum)
```

# Excellent reference books used to help make this:
- Will Pirkle - Designing Software Synthesizer Plugins in C++ 
- Will Pirkle - Designing Audio Effect Plugins in C++
- Andrist and Sehr - C++ High Performance
- Scott Meyers - Effective Modern C++
- Anthony Williams - C++ Concurrency in Action
- https://learnopengl.com/Getting-started/Hello-Window
