/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef PORT_AUDIO_HANDLER_H_
#define PORT_AUDIO_HANDLER_H_

#include "portaudio.h"

class PortAudioHandler
{
public:
	PortAudioHandler();
	virtual ~PortAudioHandler();
	PaError result() const { return m_result; }

private:
	PaError m_result;
};


#endif // PORT_AUDIO_HANDLER_H_
