/// Copyright (c) 2021. Anthony Parks. All rights reserved.
#include "mixerstream.h" 
#include "constants.h"
#include "stdio.h"
#include "math.h"
#include "graphics.h"
#include <iostream>
#include <thread>
#include "util.h"


MixerStream::MixerStream(CallbackData* userData)
	: stream(0)
	, m_callbackData(userData)
	, m_bRecording(false)
	, m_gfx(graphicsThread)
	, m_kick("C:\\drum_samples\\bd01.wav")
	, m_hat("C:\\drum_samples\\hh01.wav")
	, m_clap("C:\\drum_samples\\cp01.wav")
	, m_snare("C:\\drum_samples\\sd01.wav")
	, m_fdn(4)
{

	VoiceParams params;
	params.envParams = { 1, 250, 0, 1 };
	params.bEnableFiltLFO = true;
	params.filtLFOFreq = 0.5;
	params.filtFreq = 4000;

	m_synth.update(params);

	VoiceParams params2;
	params2.envParams = { 1, 250, 0, 1 };
	params2.osc = OscillatorType::SQUARE;
	params2.filtFreq = 800;
	params2.filtQ = 1.3;

	m_synth2.update(params2);
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
		FRAMES_PER_BUFFER,
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


void MixerStream::noteOn(int noteVal, int track)
{
	if (track == 1) {
		m_synth.noteOn(noteVal);
	}
	else if (track == 2) {
		m_synth2.noteOn(noteVal);
	}
	else if (track == 3) {
		(void)noteVal;
		m_kick.noteOn();
	}
	else if (track == 4) {
		(void)noteVal;
		m_hat.noteOn();
	}
	else if (track == 5) {
		(void)noteVal;
		m_clap.noteOn();
	}
	else if (track == 6) {
		(void)noteVal;
		m_snare.noteOn();
	}

	m_fdn.reset();

}

void MixerStream::noteOff(int noteVal, int track)
{
	if (track == 1) {
		m_synth.noteOff(noteVal);
	}
	else if (track == 2) {
		m_synth2.noteOff(noteVal);
	}
}

void MixerStream::update(VoiceParams params)
{
	m_synth.update(params);
}

int MixerStream::paCallbackMethod(const void* inputBuffer, void* outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void* userData)
{
	CallbackData* data = m_callbackData;
	
	auto* out = (float*)outputBuffer;

	(void)timeInfo; /* Prevent unused variable warnings. */
	(void)statusFlags;
	(void)inputBuffer;

	m_synth.blockRateUpdate();
	m_synth2.blockRateUpdate();

	std::array<float, FRAMES_PER_BUFFER> writeBuff = { 0.f };

	for (size_t sampIdx = 0; sampIdx < framesPerBuffer; sampIdx++)
	{
		auto output = 0.f;
		output = (1 / sqrt(2 * 6)) * (m_synth() + m_synth2() + m_kick() + m_hat() + m_snare() + m_clap());
		
		output = 0.707 * (0.8*output + 0.2*m_plate(output));
		output = clip(output);

		// write output
		*out++ = output;
		*out++ = output;

		writeBuff[sampIdx] = output;
		g_buffer[sampIdx] = static_cast<float>(output*1.0);
	}

	g_ready = true;

	while (m_callbackData->commandsToAudioCallback->size_approx()) {
		Commands cmd = *(m_callbackData->commandsToAudioCallback->peek());
		m_callbackData->commandsToAudioCallback->pop();
		switch (cmd) {
		case Commands::START_RECORDING:
			m_callbackData->server->openWrite();
			break;
		case Commands::STOP_RECORDING:
			m_callbackData->server->closeWrite();
			break;
		}

		m_callbackData->commandsFromAudioCallback->enqueue(cmd); // return command to main thread for deletion
	}

	if (m_bRecording) {
		m_callbackData->server->write(writeBuff);
	}

	return paContinue;
}

void MixerStream::record(bool bStart)
{
	if (bStart) {
		m_callbackData->commandsToAudioCallback->enqueue(Commands::START_RECORDING);
		m_bRecording = true;
	}
	else {
		m_callbackData->commandsToAudioCallback->enqueue(Commands::STOP_RECORDING);
		m_bRecording = false;
	}
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
		statusFlags,
		userData);
}

void MixerStream::paStreamFinishedMethod()
{
	std::cout << "stream finished." << std::endl;
}

void MixerStream::paStreamFinished(void* userData)
{
	return ((MixerStream*)userData)->paStreamFinishedMethod();
}
