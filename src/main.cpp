/// Copyright (c) 2021. Anthony Parks. All rights reserved.

#include <stdio.h>
#include "conio.h"

#include <math.h>
#include <thread>
#include <future>
#include <chrono>
#include <string>
#include <iostream>
#include "portaudio.h"
#include "portaudiohandler.h"
#include "mixerstream.h"

#include <cstdlib>
#include "RtMidi.h"

#include "constants.h"

#include "windows.h"



void audioThread();
void banner();

float semitoneToRatio(int semitone)
{
    semitone = (semitone < -12) ? -12 : semitone;
    semitone = (semitone > 12) ? 12 : semitone;

    return powf(2., (1.f * semitone / 12));
}

size_t g_noteVal = 0;


void audioThread()
{
    banner();
    std::cout << "PortAudio: sample rate " << SAMPLE_RATE << ", buffer size " << FRAMES_PER_BUFFER << std::endl;

    PortAudioHandler paInit;
    MixerStream stream;
	std::string prompt;

	RtMidiIn* midiin = 0;
    midiin = new RtMidiIn();
    midiin->openPort(0); 

    if (stream.open(Pa_GetDefaultOutputDevice()))
    {

        std::vector<unsigned char> message;
        VoiceParams params;

        if (stream.start()) {
			while (true) {
                std::cout << ">>> commands: stop, osc, freq, filter-freq, filter-q, filt-lfo-freq, pitch/filt-lfo-on/off, " << std::endl;
                std::cout << ">>> note, gain, exit" << std::endl;
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
                    params.freq = freq;
                }
               if (prompt == "filter-freq") {
					std::cout << ">> enter filter cutoff frequency in Hz" << std::endl;
    				std::cin >> prompt;
                    auto freq = std::stof(prompt);
                    freq = (freq > 0) ? freq : 0 ;
                    freq = (freq < 10000) ? freq : 10000;
                    params.filtFreq = freq;
                }
                if (prompt == "filter-q") {
					std::cout << ">> enter filter resonance (0 - 10)" << std::endl;
    				std::cin >> prompt;
                    auto q = std::stof(prompt);
                    q = (q > 0) ? q : 0 ;
                    q = (q < 10000) ? q : 10000;
                    params.filtQ = q;
                }
                if (prompt == "lfo-freq") {
                    std::cout << ">> enter filter LFO frequency (0 - 40)" << std::endl;
                    std::cin >> prompt;
                    auto freq = std::stof(prompt);
                    freq = (freq > 0) ? freq : 0;
                    freq = (freq < 40) ? freq : 40;
                    params.filtLFOFreq = freq;
                }
				if (prompt == "filt-lfo-on") {
                    params.bEnableFiltLFO = true;
                }
                if (prompt == "filt-lfo-off") {
                    params.bEnableFiltLFO = false;
                }
				if (prompt == "pitch-lfo-on") {
                    params.bEnablePitchLFO = true;
                }
                if (prompt == "pitch-lfo-off") {
                    params.bEnablePitchLFO = false;
                }
 
                if (prompt == "note") {
					std::cout << ">> enter midi note (21-108)" << std::endl;
                    // midi note to freq formula https://newt.phys.unsw.edu.au/jw/notes.html
                    do {
                        double stamp = 0;
                        auto nBytes = 0;
                        stamp = midiin->getMessage(&message);
                        nBytes = message.size();
                        if (nBytes == 3) {
                            int byte0 = (int)message.at(0);
                            if (byte0 == 144) {
                                g_noteVal = (int)message.at(1);
                                if (g_noteVal > 36) {
                                    float velocity = (int)message.at(2);
                                    if (velocity != 0) {
                                        stream.noteOn();
                                        float freq = pow(2.f, (g_noteVal - 69.f) / 12.f) * 440.f;
                                        freq = (freq > 0) ? freq : 0;
                                        freq = (freq < 10000) ? freq : 10000;
                                        params.freq = freq;
                                    }
                                    else {
                                        stream.noteOff();
                                    }
                                }
                            }
                        }
                    } while (!kbhit());
                    stream.noteOff();
 
                }
 
              if (prompt == "osc") {
					std::cout << ">> enter sine, saw, tri, square" << std::endl;
    				std::cin >> prompt;
                    OscillatorType osc = OscillatorType::SINE;
                    if (prompt == "sine") {
                        osc = OscillatorType::SINE;
                    }
                    if (prompt == "saw") {
                        osc = OscillatorType::SAW;
                    }
                    if (prompt == "tri") {
                        osc = OscillatorType::TRIANGLE;
                    }
                    if (prompt == "square") {
                        osc = OscillatorType::SQUARE;
                    }
                    params.osc = osc;
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

                    attMs = (attMs > 5000) ? 500 : attMs;
                    decMs = (decMs > 5000) ? 500 : decMs;
                    susdB = (susdB > 0) ? 0 : susdB;
                    relMs = (relMs > 5000) ? 500 : relMs;

                    EnvelopeParams env(attMs, decMs, susdB, relMs);
                    params.envParams = env;
                }

                stream.update(params);
                
	
	
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

