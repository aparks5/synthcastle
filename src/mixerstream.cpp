/// Copyright (c) 2021. Anthony Parks. All rights reserved.
#include "mixerstream.h" 
#include "constants.h"
#include "stdio.h"
#include "math.h"
#include "graphics.h"
#include <iostream>
#include <thread>
#include <string>
#include "util.h"


MixerStream::MixerStream(size_t fs, CallbackData* userData)
	: stream(0)
	, m_callbackData(userData)
	, m_bRecording(false)
	, m_gfx(graphicsThread)
	, m_mixer(fs)
	, m_synth(fs)
	, m_synth2(fs)
	, m_kick(fs, "C:\\drum_samples\\bd01.wav")
	, m_hat(fs, "C:\\drum_samples\\hh01.wav")
	, m_clap(fs, "C:\\drum_samples\\cp01.wav")
	, m_snare(fs, "C:\\drum_samples\\sd01.wav")
	, m_fdn(4)
	, m_plate(fs)
	, m_bLoop(false)
	, m_loopTimes(1)
	, m_bpm(120)
	, m_bPendingSynthUpdate(false)
	, m_bPendingFxUpdate(false)
	, m_activeTrackName("synth1")
	, m_bLogMidi(false)
{

	VoiceParams params;
	params.envParams = { 1, 250, 0, 1 };
	params.bEnableFiltLFO = true;
	params.filtLFOFreq = 0.5;
	params.filtFreq = 4000;

	m_synth.update(params);

	VoiceParams params2;
	params2.envParams = { 1, 0, 0, 250 };
	params2.osc = OscillatorType::SAW;
	params2.bEnableOsc2 = true;
	params2.osc2coarse = 7;
	params2.bEnableFiltLFO = true;
	params2.filtLFOFreq = 0.25;
	params2.filtFreq = 1500;
	params2.filtQ = 0.7;

	m_synth2.update(params2);

	m_synth.setName("synth1");
	m_synth2.setName("synth2");
	m_kick.setName("kick");
	m_hat.setName("hat");
	m_clap.setName("clap");
	m_snare.setName("snare");

	m_mixer.addInput(&m_synth);
	m_mixer.addInput(&m_synth2);
	m_mixer.addInput(&m_kick);
	m_mixer.addInput(&m_hat);
	m_mixer.addInput(&m_clap);
	m_mixer.addInput(&m_snare);

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


void MixerStream::noteOn(int noteVal, std::string track)
{
	if (m_bLogMidi) {
		spdlog::info("midi note: {}", noteVal);
	}

	m_mixer.noteOn(noteVal, track);
	m_fdn.reset();

}

void MixerStream::noteOff(int noteVal, std::string track)
{
	m_mixer.noteOff(noteVal, track);
}

void MixerStream::update(VoiceParams params)
{
	m_synthUpdate = params;
	m_bPendingSynthUpdate = true;
}

void MixerStream::updateTrackGainDB(std::string track, float gainDB)
{
	gainDB = clamp(gainDB, -60.f, 0.f);
	m_mixer.setInputGain(track, gainDB);
}


void MixerStream::update(FxParams fxparams)
{
	m_fxUpdate = fxparams;
	m_bPendingFxUpdate = true;
}


void MixerStream::playPattern(std::deque<NoteEvent> notes, size_t bpm)
{

	float now = 0.f;

	auto temp = NoteGenerator::sortTimeVal(notes);

	NoteEvent ev = static_cast<NoteEvent>(temp.front());

	while (!temp.empty()) {

		ev = static_cast<NoteEvent>(temp.front());
		// we are in 4/4
		float tv = ev.timeVal * 4 * 60.f * 1000.f / bpm;

		if (tv != now) {
			Sleep(tv - now);
		}

		now += tv - now;


		temp.pop_front();
		double stamp = 0;
		auto nBytes = 0;
		auto message = &ev.message;
		nBytes = message->size();
		if (nBytes == 3) {
			int byte0 = (int)message->at(0);
			auto noteVal = (int)message->at(1);
			float velocity = (int)message->at(2);
			if (byte0 == 144) {
				if (noteVal == 0)
				{
					if (tv - now > 0) {
						Sleep(tv - now);
					}
				}
				else if (velocity != 0) {
					noteOn(noteVal, ev.track);
				}
			}
			else if (byte0 == 128) {
				noteOff(noteVal, ev.track);
			}
		}


		if (tv - now < 0) {
			now = tv;
		}

	}

	// if "now" is before the end of a measure, sleep until the measure if over

	// find nearest multiple of "now" to the duration of quarter note of 4/4 at the bpm:
	float m = 1.f * 60.f * 1000.f / bpm;
    // https://math.stackexchange.com/questions/291468/how-to-find-the-nearest-multiple-of-16-to-my-given-number-n
	float nearest = (m == 0) ? now : ceil(now / m) * m;
	float endOfMeasure = nearest - now;
	Sleep(endOfMeasure);

}

void MixerStream::queueLoop(size_t numLoops, std::deque<NoteEvent> notes, size_t bpm)
{
	m_pattern = notes;
	m_loopTimes = numLoops;
	m_bpm = bpm;
	m_callbackData->commandsToAudioCallback->enqueue(Commands::START_LOOP);
}

void MixerStream::stopLoop()
{
	m_callbackData->commandsToAudioCallback->enqueue(Commands::STOP_LOOP);
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

	// <-- this gets passed to the mixer
	std::array<float, FRAMES_PER_BUFFER> writeBuff = { 0.f };

	for (size_t sampIdx = 0; sampIdx < framesPerBuffer; sampIdx++)
	{
		auto output = 0.f;

		output = m_mixer();
		
		output = clip(output);

		// write output
		*out++ = output; // left
		*out++ = output; // right

		writeBuff[sampIdx] = output;
		g_buffer[sampIdx] = static_cast<float>(output*1.0);
	}

	g_ready = true;

	while (m_callbackData->commandsToAudioCallback->size_approx()) {
		Commands cmd = *(m_callbackData->commandsToAudioCallback->peek());
		m_callbackData->commandsToAudioCallback->pop();
		switch (cmd) {
		case Commands::START_LOOP:
			m_bLoop = true;
			break;
		case Commands::STOP_LOOP:
			m_bLoop = false;
			break;

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

	m_synth.blockRateUpdate();
	m_synth2.blockRateUpdate();

	if (m_bPendingSynthUpdate) {
		m_synth.update(m_synthUpdate);
		m_bPendingSynthUpdate = false;
	}

	if (m_bPendingFxUpdate) {
		m_synth.update(m_fxUpdate);
		m_bPendingFxUpdate = false;
	}





	return paContinue;
}

void MixerStream::loop()
{
	if (m_bLoop) {
		while (m_loopTimes > 0 && m_bLoop) {
			playPattern(m_pattern, m_bpm);
			m_loopTimes--;
		}
		m_bLoop = false;
	}
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

std::vector<std::string> MixerStream::getTrackList() 
{
	return m_mixer.getInputsAsString();
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
