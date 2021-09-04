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
void banner();
void audioThread()
{
    banner();
    std::cout << "PortAudio: sample rate " << SAMPLE_RATE << ", buffer size " << FRAMES_PER_BUFFER << std::endl;

    PortAudioHandler paInit;
    MixerStream stream;
	std::string prompt;

    if (stream.open(Pa_GetDefaultOutputDevice()))
    {

		std::cout << ">> enter start to start streaming" << std::endl;
        while (true) {
            std::cout << std::endl << std::endl;
            std::cin >> prompt;
            if (prompt == "start") {
                break;
            }
        }
 
        if (stream.start()) {
			while (true) {
                std::cout << ">> commands: start, stop, osc, freq, gain, bpm, exit" << std::endl;
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
                if (prompt == "osc") {
					std::cout << ">> enter sine, saw, tri, square" << std::endl;
    				std::cin >> prompt;
                    Oscillator osc = Oscillator::SINE;
                    if (prompt == "sine") {
                        osc = Oscillator::SINE;
                    }
                    if (prompt == "saw") {
                        osc = Oscillator::SAW;
                    }
                    if (prompt == "tri") {
                        osc = Oscillator::TRIANGLE;
                    }
                    if (prompt == "square") {
                        osc = Oscillator::SQUARE;
                    }
                    stream.updateOsc(osc);
                }
                if (prompt == "env") {
                    std::cout << ">> enter adsr envelope parameters (attack ms, decay ms, sustain dB (< 0), release ms) (e.g. 250 10 -10 500) " << std::endl;
                    std::vector<std::string> param;
                    size_t paramCount = 0;
                    std::string envP;
                    while (paramCount < 4 && std::cin >> envP) {
                        param.push_back(envP);
                        paramCount++;
                    }
                    size_t attMs = 0;
                    size_t decMs = 0;
                    int susdB = 0;
                    size_t relMs = 0;

                    std::sscanf(param[0].c_str(), "%zu", &attMs);
                    std::sscanf(param[1].c_str(), "%zu", &decMs);
                    std::sscanf(param[2].c_str(), "%d", &susdB);
                    std::sscanf(param[3].c_str(), "%zu", &relMs);

                    attMs = (attMs > 500) ? 500 : attMs;
                    decMs = (decMs > 500) ? 500 : decMs;
                    susdB = (susdB > 0) ? 0 : susdB;
                    relMs = (relMs > 500) ? 500 : relMs;

                    EnvelopeParams params(attMs, decMs, susdB, relMs);
                    stream.updateEnv(params);
                }
                
	
	
			}
            stream.stop();
        }

        stream.close();
    }
}
void banner()
{
    std::cout << R"(
  _______     ___   _ _______ _    _  _____           _____ _______ _      ______ 
 / ____\ \   / / \ | |__   __| |  | |/ ____|   /\    / ____|__   __| |    |  ____|
| (___  \ \_/ /|  \| |  | |  | |__| | |       /  \  | (___    | |  | |    | |__   
 \___ \  \   / | . ` |  | |  |  __  | |      / /\ \  \___ \   | |  | |    |  __|  
 ____) |  | |  | |\  |  | |  | |  | | |____ / ____ \ ____) |  | |  | |____| |____ 
|_____/   |_|  |_| \_|  |_|  |_|  |_|\_____/_/    \_\_____/   |_|  |______|______|
    )" << std::endl;
}
int main(void);

int main(void)
{
    std::thread audio(audioThread);
    audio.join();
	return 0;
}

