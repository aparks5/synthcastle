﻿/// Copyright (c) 2021. Anthony Parks. All rights reserved.

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


void midiThread();



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

		std::cout << ">> enter start to start streaming" << std::endl;
        while (true) {
            std::cout << std::endl << std::endl;
            std::cin >> prompt;
            if (prompt == "start") {
                break;
            }
        }

        std::vector<unsigned char> message;

        if (stream.start()) {
			while (true) {
                std::cout << ">>> commands: start, stop, osc, freq, filter-freq, filter-q, filt-lfo-freq, pitch/filt-lfo-on/off, " << std::endl;
                std::cout << ">>> note, gain, bpm, exit" << std::endl;
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
                // TODO: add oscillator 2 semitone ratio, mix, type
                /*
                if (prompt == "osc2-on") {
					std::cout << ">> enter semitone ratio (-12 to 12)" << std::endl;
    				std::cin >> prompt;
                    semitoneToRatio(std::stod(prompt));
                }
                if (prompt == "osc2-off") {
					std::cout << ">> enter semitone ratio (-12 to 12)" << std::endl;
    				std::cin >> prompt;
                    semitoneToRatio(std::stod(prompt));
                }
                if (prompt == "osc2-coarse") {
					std::cout << ">> enter semitone ratio (-12 to 12)" << std::endl;
    				std::cin >> prompt;
                    semitoneToRatio(std::stod(prompt));
                }
                if (prompt == "osc2-fine") {
					std::cout << ">> enter semitone ratio (-12 to 12)" << std::endl;
    				std::cin >> prompt;
                    semitoneToRatio(std::stod(prompt));
                }
                if (prompt == "osc2-mix") {
					std::cout << ">> enter semitone ratio (-12 to 12)" << std::endl;
    				std::cin >> prompt;
                    semitoneToRatio(std::stod(prompt));
                }
                if (prompt == "osc2-type") {
					std::cout << ">> enter semitone ratio (-12 to 12)" << std::endl;
    				std::cin >> prompt;
                    semitoneToRatio(std::stod(prompt));
                }
	
	
				*/
                if (prompt == "filter-freq") {
					std::cout << ">> enter filter cutoff frequency in Hz" << std::endl;
    				std::cin >> prompt;
                    auto freq = std::stof(prompt);
                    freq = (freq > 0) ? freq : 0 ;
                    freq = (freq < 10000) ? freq : 10000;
                    stream.updateFilterCutoff(freq);
                }
                if (prompt == "filter-q") {
					std::cout << ">> enter filter resonance (0 - 10)" << std::endl;
    				std::cin >> prompt;
                    auto q = std::stof(prompt);
                    q = (q > 0) ? q : 0 ;
                    q = (q < 10000) ? q : 10000;
                    stream.updateFilterResonance(q);
                }
                if (prompt == "lfo-freq") {
                    std::cout << ">> enter filter LFO frequency (0 - 40)" << std::endl;
                    std::cin >> prompt;
                    auto freq = std::stof(prompt);
                    freq = (freq > 0) ? freq : 0;
                    freq = (freq < 40) ? freq : 40;
                    stream.updateLfoRate(freq);
                }
				if (prompt == "filt-lfo-on") {
                     stream.enableFiltLFO();
                }
                if (prompt == "filt-lfo-off") {
                     stream.disableFiltLFO();
                }
				if (prompt == "pitch-lfo-on") {
                     stream.enablePitchLFO();
                }
                if (prompt == "pitch-lfo-off") {
                     stream.disablePitchLFO();
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
                                        stream.updateFreq(freq);
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

                    attMs = (attMs > 5000) ? 500 : attMs;
                    decMs = (decMs > 5000) ? 500 : decMs;
                    susdB = (susdB > 0) ? 0 : susdB;
                    relMs = (relMs > 5000) ? 500 : relMs;

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

