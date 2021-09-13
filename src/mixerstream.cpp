/// Copyright (c) 2021. Anthony Parks. All rights reserved.
#include "mixerstream.h" 
#include "constants.h"
#include "stdio.h"
#include "math.h"
#include "graphics.h"
#include <iostream>
#include <thread>

MixerStream::MixerStream()
	: stream(0)
	, m_gfx(graphicsThread)
	, delay(SAMPLE_RATE, 1.f)
	, delay2(SAMPLE_RATE, 1.f)
	, chorus(SAMPLE_RATE)
	, lastActiveVoice(0)
{
	auto nVoices = 4;

	for (auto i = 0; i < nVoices; i++) {
		std::shared_ptr<Voice> v = std::make_shared<Voice>();
		m_voices.push_back(v);
	}


	delay.update(250);
	delay2.update(133);
	chorus.depth(1.f);
	chorus.rate(0.3f);
}

void MixerStream::noteOn(float freq)
{
	auto activeVoices = 0;
	for (auto voice : m_voices) {
		if (voice->active()) {
			activeVoices++;
		}
		else { // we found an unused voice
			voice->updateFreq(freq);
			voice->noteOn();
			lastActiveVoice++;
			if (lastActiveVoice>= 4) {
				lastActiveVoice = 0;
			}
			return;
		}
	}

	// all voices are exhausted, we need to turn one off
	if (activeVoices >= m_voices.size()) {
		m_voices[lastActiveVoice]->updateFreq(freq);
		m_voices[lastActiveVoice]->noteOn();
		lastActiveVoice++;
		if (lastActiveVoice >= 4) {
			lastActiveVoice = 0;
		}
	}

}

void MixerStream::noteOff()
{
	for (auto voice : m_voices) {
		voice->noteOff();
	}
	
}


bool MixerStream::open(PaDeviceIndex index)
{
	PaStreamParameters outputParameters;

	outputParameters.device = index;
	if (outputParameters.device == paNoDevice) {
		return false;
	}

	const PaDeviceInfo* pInfo = Pa_GetDeviceInfo(index);
	if (pInfo != 0)
	{
		printf("Output device name: '%s'\r", pInfo->name);
	}

	outputParameters.channelCount = 2;       /* stereo output */
	outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	PaError err = Pa_OpenStream(
		&stream,
		NULL, /* no input */
		&outputParameters,
		SAMPLE_RATE,
		paFramesPerBufferUnspecified,
		paClipOff,      /* we won't output out of range samples so don't bother clipping them */
		&MixerStream::paCallback,
		this            /* Using 'this' for userData so we can cast to MixerStream* in paCallback method */
	);

	if (err != paNoError)
	{
		/* Failed to open stream to device !!! */
		return false;
	}

	err = Pa_SetStreamFinishedCallback(stream, &MixerStream::paStreamFinished);

	if (err != paNoError)
	{
		Pa_CloseStream(stream);
		stream = 0;

		return false;
	}

	return true;
}

bool MixerStream::close()
{
	if (stream == 0)
		return false;

	PaError err = Pa_CloseStream(stream);
	stream = 0;

	m_gfx.join();

	return (err == paNoError);

}

bool MixerStream::start()
{
	if (stream == 0)
		return false;

	PaError err = Pa_StartStream(stream);

	return (err == paNoError);
}

bool MixerStream::stop()
{
	if (stream == 0) {
		return false;
	}

	PaError err = Pa_StopStream(stream);

	return (err == paNoError);
}

void MixerStream::update(VoiceParams params)
{
	for (auto voice : m_voices) {
		voice->update(params);
	}
}

int MixerStream::paCallbackMethod(const void* inputBuffer, void* outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags)
{
	auto* out = (float*)outputBuffer;

	(void)timeInfo; /* Prevent unused variable warnings. */
	(void)statusFlags;
	(void)inputBuffer;

	for (auto voice : m_voices) {
		voice->modUpdate();
	}

	for (size_t sampIdx = 0; sampIdx < framesPerBuffer; sampIdx++)
	{
		auto output = 0.f;

		for (auto voice : m_voices) {
			output += voice->apply() / m_voices.capacity();
		}

//		output = (1 / (3 * .707f)) * (output + 0.5f * delay(output) + 0.5 * delay2(output));
		output = 0.5*output + 0.5*chorus(output);
		

		// write output
		*out++ = output;
		*out++ = output;
		g_buffer[sampIdx] = static_cast<float>(output*1.0);
	}


	g_ready = true;


	return paContinue;
}

int MixerStream::paCallback(const void* inputBuffer, void* outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void* userData)
{
	/* Here we cast userData to MixerStream* type so we can call the instance method paCallbackMethod, we can do that since
	   we called Pa_OpenStream with 'this' for userData */
	return ((MixerStream*)userData)->paCallbackMethod(inputBuffer, outputBuffer,
		framesPerBuffer,
		timeInfo,
		statusFlags);
}

void MixerStream::paStreamFinishedMethod()
{
	std::cout << "stream finished." << std::endl;
}

void MixerStream::paStreamFinished(void* userData)
{
	return ((MixerStream*)userData)->paStreamFinishedMethod();
}


