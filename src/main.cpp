/// Copyright (c) 2021. Anthony Parks. All rights reserved.

#include <stdio.h>
#include <math.h>
#include <thread>
#include <string>
#include <iostream>
#include "portaudio.h"
#include "portaudiohandler.h"
#include "mixerstream.h"

#include "constants.h"

void audioThread();
void audioThread()
{
    MixerStream stream;

    printf("PortAudio Test: output triangle wave. SR = %d, BufSize = %d\n", SAMPLE_RATE, FRAMES_PER_BUFFER);

    PortAudioHandler paInit;

	std::string prompt;

    if (stream.open(Pa_GetDefaultOutputDevice()))
    {
        while (true) {
            std::cout << std::endl << std::endl;
            std::cout << ">> enter start to start streaming" << std::endl;
            std::cin >> prompt;
            if (prompt == "start") {
                break;
            }
        }
 
        if (stream.start()) {
			while (true) {
                std::cout << ">> commands: start, stop, freq, gain, bpm, exit" << std::endl;
				std::cin >> prompt;
                if (prompt == "stop") {
                    stream.stop();
                }
                if (prompt == "start") {
                    stream.start();
                }
                if (prompt == "exit") {
                    break;
                }
                if (prompt == "freq") {
					std::cout << ">> enter frequency in Hz" << std::endl;
    				std::cin >> prompt;
                    auto freq = std::stof(prompt);
                    freq = (freq > 0) ? freq : 0 ;
                    freq = (freq < 10000) ? freq : 10000;
                    stream.updateFreq(freq);
                }
                if (prompt == "gain") {
					std::cout << ">> enter gain in dB (0 to -60)" << std::endl;
    				std::cin >> prompt;
                    auto gain = std::stod(prompt);
                    gain = (gain < -60) ? -60 : gain;
                    gain = (gain > 0) ? 0 : gain;
                    stream.updateGain(gain);
                }
                if (prompt == "bpm") {
					std::cout << ">> enter bpm (40 to 200)" << std::endl;
    				std::cin >> prompt;
                    auto bpm = std::stod(prompt);
                    bpm = (bpm < 40) ? 40 : bpm;
                    bpm = (bpm > 200) ? 200 : bpm ;
                    stream.updateBPM(bpm);
                }
	
	
			}
            stream.stop();
        }

        stream.close();
    }
}
int main(void);

int main(void)
{
    std::thread audio(audioThread);
    audio.join();
	return 0;
}

