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
	, durationCounter(0)
	, m_osc(Oscillator::SINE)
	, m_moogFilter(SAMPLE_RATE)
	, m_filtFreq(0.f)
	, m_bParamChanged(false)
	, m_prevSample(0.f)
{
	EnvelopeParams env(250, 0, 0, 500);
	m_env.setParams(env);

	m_moogFilter.freq(1000.f);
	m_filtFreq = 1000.f;
	m_moogFilter.q(3.f);
	m_lfo.freq(1.f);
	m_lfo.update();
	
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
	m_saw.freq(freq);
	m_saw2.freq(freq * 1.3348);
	m_tri.freq(freq);
	m_square.freq(freq);
	m_sine.freq(freq);
	m_bParamChanged = true;
}

void MixerStream::updateGain(int gaindB)
{
	m_gain.setGaindB(gaindB);
	m_bParamChanged = true;
}

void MixerStream::updateBPM(size_t bpm)
{
	m_metronome.bpm(bpm);
	m_bParamChanged = true;
}


void MixerStream::updateOsc(Oscillator osc)
{
	m_osc = osc;
	m_bParamChanged = true;
}

void MixerStream::updateEnv(EnvelopeParams params)
{
	m_envParams = params;
	m_bParamChanged = true;
}

void MixerStream::enableFiltLFO()
{
	m_bEnableFilterLFO = true;
}

void MixerStream::disableFiltLFO()
{
	m_bEnableFilterLFO = false;
}

void MixerStream::noteOn()
{
	m_env.noteOn();
	m_env.reset();
	m_tri.reset();
	m_saw.reset();
	m_sine.reset();
	m_square.reset();
}

void MixerStream::noteOff()
{
	m_env.noteOff();
}

void MixerStream::updateLfoRate(double freq)
{
	m_lfo.freq(freq);
	m_bParamChanged = true;
}

void MixerStream::updateFilterCutoff(double freq)
{
	m_moogFilter.freq(freq);
	m_filtFreq = freq;
}

void MixerStream::updateFilterResonance(double q)
{
	m_moogFilter.q(q);
}



void MixerStream::processUpdates()
{

	if (m_bParamChanged) {
		m_saw.update();
		m_saw2.update();
		m_lfo.update();
		m_tri.update();
		m_square.update();
		m_sine.update();
		m_env.setParams(m_envParams);

		m_bParamChanged = false;
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

	auto output = 0.0f;
	
    // duration = 1s
	auto samplesPerDuration = SAMPLE_RATE;

	processUpdates();
	double envGain = 0;

	if (m_bEnableFilterLFO) {
		float filtLfo = (1 + m_lfo.generate()) * 0.5f;
		m_moogFilter.freq(m_filtFreq * filtLfo);
	}
	else {
		m_moogFilter.freq(m_filtFreq);
	}

	for (size_t sampIdx = 0; sampIdx < framesPerBuffer; sampIdx++)
	{

		m_metronome.tick();

		// generate sample
		envGain = m_env.apply(1);
		m_gain.setGainf(envGain);
		oscillate(output);
		if (m_bEnableFilterLFO) {
			m_lfo.generate();
		}
     	m_moogFilter.apply(&output, 1);
		output = m_gain.apply(output);
	
		// write output
		*out++ = output;
		*out++ = output;
		g_buffer[sampIdx] = static_cast<float>(output*1.0f);
		m_prevSample = output;
	}




	g_ready = true;

	return paContinue;
}

void MixerStream::oscillate(float& output)
{
	switch (m_osc) {
	case Oscillator::SAW:
		output = (m_saw.generate() + m_saw2.generate()) * 0.5f;
		break;
	case Oscillator::SINE:
		output = m_sine.generate();
		break;
	case Oscillator::TRIANGLE:
		output = m_tri.generate();
		break;
	case Oscillator::SQUARE:
		output = m_square.generate();
		break;
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


