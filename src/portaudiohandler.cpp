/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#include "portaudiohandler.h"

PortAudioHandler::PortAudioHandler()
	: m_result(Pa_Initialize())
{
}

PortAudioHandler::~PortAudioHandler()
{
	if (m_result == paNoError)
	{
		Pa_Terminate();
	}
}