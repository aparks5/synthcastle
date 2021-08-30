/// Copyright (c) 2021. Anthony Parks. All rights reserved.

#include <stdio.h>
#include <math.h>
#include <string>
#include <iostream>
#include "portaudio.h"
#include "portaudiohandler.h"
#include "mixerstream.h"

constexpr auto SAMPLE_RATE = (44100);
constexpr auto FRAMES_PER_BUFFER = (256);

int main(void);

int main(void)
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
                std::cout << ">> commands: start, stop, freq, exit" << std::endl;
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
                    stream.update(freq);
                }
			}
            stream.stop();
        }

        stream.close();
    }

	return 0;
}

