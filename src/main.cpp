/// Copyright (c) 2021. Anthony Parks. All rights reserved.

#include <stdio.h>
#include <math.h>
#include <thread>
#include <future>
#include <chrono>
#include <string>
#include <iostream>
#include <cstdlib>
#include <queue>

#include "portaudio.h"
#include "portaudiohandler.h"
#include "windows.h"
#include "conio.h"

#include "constants.h"
#include "midi.h"
#include "prompt.h"
#include "util.h"



void audioThread();
void banner();

void audioThread()
{
	banner();
	std::cout << "PortAudio: sample rate " << SAMPLE_RATE << ", buffer size " << FRAMES_PER_BUFFER << std::endl;

	PortAudioHandler paInit;
	MixerStream stream;
	MIDI midi(stream);
	Prompt prompt(stream);

if (stream.open(Pa_GetDefaultOutputDevice()))
	{

		std::vector<unsigned char> message;
		VoiceParams params;

		if (stream.start()) {
			prompt.open();
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
