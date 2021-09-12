/// Copyright (c) 2021. Anthony Parks. All rights reserved.

#include <stdio.h>
#include "conio.h"

#include <math.h>
#include <thread>
#include <future>
#include <chrono>
#include <string>
#include <iostream>
#include <cstdlib>

#include "portaudio.h"
#include "portaudiohandler.h"
#include "RtMidi.h"
#include "windows.h"

#include "constants.h"
#include "mixerstream.h"
#include "util.h"

void audioThread();
void banner();

typedef struct
{
    MixerStream* stream;
} MIDIUserData;

static MIDIUserData midiUserData;

void midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData)
{
    MIDIUserData* pUserData = (MIDIUserData*)userData;
    MixerStream* stream = pUserData->stream;
	// midi note to freq formula https://newt.phys.unsw.edu.au/jw/notes.html
	double stamp = 0;
	auto nBytes = 0;
	nBytes = message->size();
	if (nBytes == 3) {
		int byte0 = (int)message->at(0);
		if (byte0 == 144) {
			auto noteVal = (int)message->at(1);
			if (noteVal > 36) {
				float velocity = (int)message->at(2);
				if (velocity != 0) {
					float freq = pow(2.f, (noteVal - 69.f) / 12.f) * 440.f;
					freq = (freq > 0) ? freq : 0;
					freq = (freq < 10000) ? freq : 10000;
					stream->noteOn(freq);
				}
				else {
					if (deltatime > 0.03) {
						stream->noteOff();
					}
				}
			}
		}
	}
}


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
	midiUserData.stream = &stream;
	midiin->setCallback(&midiCallback, &midiUserData);

	if (stream.open(Pa_GetDefaultOutputDevice()))
	{

		std::vector<unsigned char> message;
		VoiceParams params;

		if (stream.start()) {
			while (true) {
				std::cout << ">>> commands: stop, osc, freq, filter-freq, filter-q, filt-lfo-freq, pitch/filt-lfo-on/off, " << std::endl;
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
					freq = (freq > 0) ? freq : 0;
					freq = (freq < 10000) ? freq : 10000;
					params.freq = freq;
				}
				if (prompt == "filt-freq") {
					std::cout << ">> enter filter cutoff frequency in Hz" << std::endl;
					std::cin >> prompt;
					auto freq = std::stof(prompt);
					freq = (freq > 0) ? freq : 0;
					freq = (freq < 10000) ? freq : 10000;
					params.filtFreq = freq;
				}
				if (prompt == "filt-q") {
					std::cout << ">> enter filter resonance (0 - 10)" << std::endl;
					std::cin >> prompt;
					auto q = std::stof(prompt);
					q = (q > 0) ? q : 0;
					q = (q < 10000) ? q : 10000;
					params.filtQ = q;
				}
				if (prompt == "filt-lfo-freq") {
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
				if (prompt == "osc2-enable") {
					params.bEnableOsc2 = true;
				}

				if (prompt == "osc2-disable") {
					params.bEnableOsc2 = false;
				}
				if (prompt == "osc2") {
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
					params.osc2 = osc;
				}
				if (prompt == "osc2-coarse") {
					std::cin >> prompt;
					auto coarse = std::stof(prompt);
					coarse = (coarse < -24.) ? -24 : coarse;
					coarse = (coarse > 24.) ? 24 : coarse;
					params.osc2coarse = coarse;
				}
				if (prompt == "osc2-fine") {
					std::cin >> prompt;
					auto fine = std::stof(prompt);
					fine = (fine < -1.) ? -1. : fine;
					fine = (fine > 1.) ? 1. : fine;
					params.osc2fine = fine;
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

