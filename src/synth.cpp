#include "synth.h"

Synth::Synth()
	: delay(SAMPLE_RATE, 1.f)
	, delay2(SAMPLE_RATE, 1.f)
	, delay3(SAMPLE_RATE, 1.f)
	, chorus(SAMPLE_RATE, 0.2, 0.8, 0.0)
	, lastActiveVoice(0)
{
	auto nVoices = 8;

	for (auto i = 0; i < nVoices; i++) {
		std::shared_ptr<Voice> v = std::make_shared<Voice>();
		m_voices.push_back(v);
	}

	delay.update(333, 0.5f);

}

void Synth::noteOn(int midiNote)
{
	auto activeVoices = 0;
	for (auto voice : m_voices) {
		if (voice->active()) {
			activeVoices++;
		}
		else { // we found an unused voice
			voice->noteOn(midiNote);
			lastActiveVoice++;
			if (lastActiveVoice>= 4) {
				lastActiveVoice = 0;
			}
			return;
		}
	}

	// all voices are exhausted, we need to turn one off
	if (activeVoices >= m_voices.size()) {
		m_voices[lastActiveVoice]->noteOn(midiNote);
		lastActiveVoice++;
		if (lastActiveVoice >= 4) {
			lastActiveVoice = 0;
		}
	}

}

void Synth::noteOff(int midiNote)
{
	// which voice should we turn off?
	for (auto voice : m_voices) {
		voice->noteOff(midiNote);
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
		output += voice->apply() * (1 / sqrt(m_voices.capacity() * 2));
	}

	int fxCount = 0;

	if (m_fxParams.bEnableChorus) {
		output += chorus(output);
	}
	if (m_fxParams.bEnableDelay1) {
		float temp = output;
		output += delay();
		delay.write(temp);
	}
	if (m_fxParams.bEnableDelay2) {
		output += delay2();
		delay2.write(output);
	}
	if (m_fxParams.bEnableReverb) {
		output += reverb(output);
	}


	output *= 1. / sqrt(2 * (fxCount + 1));


	return output;

}






