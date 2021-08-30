# synthcastle
an interactive text-based synthesizer in modern c++

like building sandcastles in sound

### Description

This project aims to create a musical instrument and prototyping ground for new audio algorithms.

I have only built this on Windows 10, compiled in Visual Studio.

It uses cmake and PortAudio.

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
