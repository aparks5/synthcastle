#include "synth.h"
#include <iostream>
#include <spdlog/spdlog.h>

Synth::Synth(size_t fs)
	: Module(fs)
	, chorus(fs, 0.2, 0.8, 0.0)
	, delay(fs)
	, delay2(fs, 1.f)
	, delay3(fs, 1.f)
	, bitcrush(fs)
	, reverb(fs)
	, pitch(fs)
	, lastActiveVoice(0)
{
	auto nVoices = 8;

	for (auto i = 0; i < nVoices; i++) {
		std::shared_ptr<Voice> v = std::make_shared<Voice>(fs);
		m_voices.push_back(v);
	}

	delay.update(333);

}

void Synth::noteOn(size_t noteVal)
{
	auto activeVoices = 0;
	for (auto voice : m_voices) {
		if (voice->active()) {
			activeVoices++;
		}
		else { // we found an unused voice
			voice->noteOn(noteVal);
			lastActiveVoice++;
			if (lastActiveVoice>= 4) {
				lastActiveVoice = 0;
			}
			return;
		}
	}

	// all voices are exhausted, we need to turn one off
	if (activeVoices >= m_voices.size()) {
		m_voices[lastActiveVoice]->noteOn(noteVal);
		lastActiveVoice++;
		if (lastActiveVoice >= 4) {
			lastActiveVoice = 0;
		}
	}

}

void Synth::noteOff(size_t noteVal)
{
	for (auto voice : m_voices) {
		voice->noteOff(noteVal);
	}
}

void Synth::update(VoiceParams params)
{
	for (auto voice : m_voices) {
		voice->update(params);
	}
}

void Synth::update(FxParams fxParams)
{
	m_fxParams = fxParams;
	delay.update(m_fxParams.delay1time);
}

void Synth::blockRateUpdate()
{
	for (auto voice : m_voices) {
		voice->modUpdate();
	}
}

float Synth::operator()() 
{
	auto output = 0.f;

	for (auto voice : m_voices) {
		output += (*voice)() * (1 / sqrt(2));
	}

	int fxCount = 0;

	if (m_fxParams.bEnableBitcrusher) {
		bitcrush.bits(4);
		output += bitcrush(output);
		fxCount++;
	}

	if (m_fxParams.bEnableChorus) {
		output += chorus(output);
		fxCount++;
	}
	if (m_fxParams.bEnableDelay1) {
		float temp = output;
		output += delay(temp);
		fxCount++;
	}
	if (m_fxParams.bEnableDelay2) {
		output += delay2();
		delay2.write(output);
		fxCount++;
	}
	if (m_fxParams.bEnableReverb) {
		output = 0.8*output + 0.1*reverb(output);
		fxCount++;
	}
	output = 0.5f*output + 0.5f*pitch(output);
	fxCount++;


	output *= 1. / sqrt(2 * (fxCount + 1));


	return output;

}






