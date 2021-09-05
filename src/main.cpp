/// Copyright (c) 2021. Anthony Parks. All rights reserved.

#include <stdio.h>
#include <math.h>
#include <thread>
#include <string>
#include <iostream>
#include "portaudio.h"
#include "portaudiohandler.h"
#include "mixerstream.h"

#include <cstdlib>
#include "RtMidi.h"

#include "constants.h"

void audioThread();
void banner();

size_t g_noteVal = 0;


void midiThread();

bool chooseMidiPort(RtMidiIn* rtmidi);

void midiCallback(double deltatime, std::vector< unsigned char >* message, void*/*userData*/)
{
    unsigned int nBytes = message->size();
    for (unsigned int i = 0; i < nBytes; i++) {
        auto midiByte = (int)message->at(i);
        if (midiByte != 248) {
            std::cout << "Byte " << i << " = " << midiByte << ", ";
            if (nBytes > 0) {
                std::cout << "stamp = " << deltatime << std::endl;
            }
        }
   }

	if ((int)message->at(0) == 144) {
        g_noteVal = (size_t)message->at(1);
        // velocity = (size_t)message->at(2);
	}
 
}

void midiThread()
{
	RtMidiIn* midiin = 0;

    try {

        // RtMidiIn constructor
        midiin = new RtMidiIn();

        // Call function to select port.
        if (chooseMidiPort(midiin) == false) goto cleanup;

        // Set our callback function.  This should be done immediately after
        // opening the port to avoid having incoming messages written to the
        // queue instead of sent to the callback function.
        midiin->setCallback(&midiCallback);

        // Don't ignore sysex, timing, or active sensing messages.
        midiin->ignoreTypes(false, false, false);

        std::cout << "\nReading MIDI input ... press <enter> to quit.\n";
        char input;
        std::cin.get(input);

    }
    catch (RtMidiError& error) {
        error.printMessage();
    }

cleanup:

    delete midiin;

}

bool chooseMidiPort(RtMidiIn* rtmidi)
{
    std::cout << "\nWould you like to open a virtual input port? [y/N] ";

    std::string keyHit;
    std::getline(std::cin, keyHit);
    if (keyHit == "y") {
        rtmidi->openVirtualPort();
        return true;
    }

    std::string portName;
    unsigned int i = 0, nPorts = rtmidi->getPortCount();
    if (nPorts == 0) {
        std::cout << "No input ports available!" << std::endl;
        return false;
    }

    if (nPorts == 1) {
        std::cout << "\nOpening " << rtmidi->getPortName() << std::endl;
    }
    else {
        for (i = 0; i < nPorts; i++) {
            portName = rtmidi->getPortName(i);
            std::cout << "  Input port #" << i << ": " << portName << '\n';
        }

        do {
            std::cout << "\nChoose a port number: ";
            std::cin >> i;
        } while (i >= nPorts);
        std::getline(std::cin, keyHit);  // used to clear out stdin
    }

    rtmidi->openPort(i);

    return true;
}



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
                if (g_noteVal != 0) {
                    auto freq = pow(2, (g_noteVal - 69) / 12) * 440.f;
                    freq = (freq > 0) ? freq : 0 ;
                    freq = (freq < 10000) ? freq : 10000;
                    stream.updateFreq(freq);


                }
                std::cout << ">> commands: start, stop, osc, freq, note, gain, bpm, exit" << std::endl;
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
                if (prompt == "note") {
					std::cout << ">> enter midi note (21-108)" << std::endl;
                    // midi note to freq formula https://newt.phys.unsw.edu.au/jw/notes.html
    				std::cin >> prompt;
                    auto note = std::stod(prompt);
                    auto freq = pow(2, (note - 69) / 12) * 440.f;
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
    midiThread();
    audio.join();
	return 0;
}

