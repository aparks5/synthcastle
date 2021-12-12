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
#include <memory>
#include "controller.h"



void uiThread(std::shared_ptr<MixerStream> stream);
void audioThread(std::shared_ptr<MixerStream> stream);
void controlThread(std::shared_ptr<MixerStream> stream);
void banner();



void uiThread(std::shared_ptr<MixerStream> stream)
{
	MIDI midi(stream);
	Prompt prompt(stream);
	prompt.open();
	// infinite loop, todo: maybe poll thread for exit code
	while (1) {};
}

void controlThread(std::shared_ptr<MixerStream> stream)
{
	Controller m_control(stream);
	while (1) {
		while (stream->shouldLoop()) {
			m_control.loop();
		}
	}
	
}


void audioThread(std::shared_ptr<MixerStream> stream)
{
	std::cout << "PortAudio: sample rate " << SAMPLE_RATE << ", buffer size " << FRAMES_PER_BUFFER << std::endl;

	if (stream->open(Pa_GetDefaultOutputDevice()))
	{

		std::vector<unsigned char> message;
		VoiceParams params;

		bool bStarted = false;
		bStarted = stream->start();
		if (!bStarted) {
			stream->close();
		}
		else {
			// infinite loop, todo: maybe poll thread for exit code
			while (1);
		}
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

	banner();


	PortAudioHandler paInit;
	CallbackData userData;
	std::shared_ptr<moodycamel::ReaderWriterQueue<Commands>> sendCommands = std::make_shared<moodycamel::ReaderWriterQueue<Commands>>(5);
	std::shared_ptr<moodycamel::ReaderWriterQueue<Commands>> receiveCommands = std::make_shared<moodycamel::ReaderWriterQueue<Commands>>(5);
	std::shared_ptr<IOServer> server = std::make_shared<IOServer>();
	userData.commandsFromAudioCallback = receiveCommands;
	userData.commandsToAudioCallback = sendCommands;
	userData.server = server;
	std::shared_ptr<MixerStream> stream = std::make_shared<MixerStream>(SAMPLE_RATE, &userData);
	std::thread ui(uiThread, stream);
	std::thread audio(audioThread, stream);
	std::thread control(controlThread, stream);
	control.join();
	audio.detach();
	ui.join();
	return 0;
}
