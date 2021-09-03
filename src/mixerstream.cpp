/// Copyright (c) 2021. Anthony Parks. All rights reserved.
#include "mixerstream.h" 
#include "stdio.h"
#include "math.h"
#include "graphics.h"
#include <iostream>
#include <thread>

constexpr auto NUM_SECONDS = (2);
constexpr auto SAMPLE_RATE = (44100);
constexpr auto FRAMES_PER_BUFFER = (256);

MixerStream::MixerStream()
	: stream(0)
	, m_gfx(graphicsThread)
	, durationCounter(0)
{
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

void MixerStream::updateFreq(float freq)
{
	m_saw.setFrequency(freq);
}

void MixerStream::updateGain(int gaindB)
{
	m_gain.setGaindB(gaindB);
}



void MixerStream::processUpdates()
{
	m_saw.update();
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

	auto output = 0.0f;
	
	processUpdates();

	// metronome - 60 BPM
	auto samplesPerBeat = SAMPLE_RATE * 60 / 60;
    // duration = 250ms
	auto samplesPerDuration = SAMPLE_RATE * 0.250;

	for (size_t sampIdx = 0; sampIdx < framesPerBuffer; sampIdx++)
	{
		
		if (m_metronome.isOnBeat()) {
			durationCounter = 0;
		}

		m_metronome.tick();
		if (durationCounter < samplesPerDuration) {
			output = m_saw.generate();
			output = m_gain.apply(output);
			durationCounter++;
		}
		else {
			output = 0;
		}
	

		// get samples from system
		*out++ = output;
		*out++ = output;
		g_buffer[sampIdx] = static_cast<float>(output*1.0f);
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


