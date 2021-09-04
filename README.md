Welcome to...
```
  _______     ___   _ _______ _    _  _____           _____ _______ _      ______ 
 / ____\ \   / / \ | |__   __| |  | |/ ____|   /\    / ____|__   __| |    |  ____|
| (___  \ \_/ /|  \| |  | |  | |__| | |       /  \  | (___    | |  | |    | |__   
 \___ \  \   / | . ` |  | |  |  __  | |      / /\ \  \___ \   | |  | |    |  __|  
 ____) |  | |  | |\  |  | |  | |  | | |____ / ____ \ ____) |  | |  | |____| |____ 
|_____/   |_|  |_| \_|  |_|  |_|  |_|\_____/_/    \_\_____/   |_|  |______|______|
                                                                                  
```

# synthcastle
an interactive text-based synthesizer in modern c++

like building sandcastles in sound





### Description

This project aims to create a musical instrument and prototyping ground for new audio algorithms.

I have only built this on Windows 10, compiled in Visual Studio.

It uses cmake, PortAudio, and OpenGL (GLAD + GLFW).

The syntax once you run `main.cpp` is very simple:

```
PortAudio Test: output triangle wave. SR = 44100, BufSize = 256
Output device name: 'Speakers (Elektron Digitakt)'

>> enter start to start streaming
start
>> commands: start, stop, freq, exit
stop
stream finished.
>> commands: start, stop, freq, exit
start
>> commands: start, stop, freq, exit
freq
>> enter frequency in Hz
50
>> commands: start, stop, freq, exit
stop
stream finished.
>> commands: start, stop, freq, exit
exit
```

# Excellent reference books used to help make this:
Will Pirkle - Designing Software Synthesizer Plugins in C++ 
Will Pirkle - Designing Audio Effect Plugins in C++
Andrist and Sehr - C++ High Performance
Scott Meyers - Effective Modern C++
Anthony Williams - C++ Concurrency in Action
https://learnopengl.com/Getting-started/Hello-Window
