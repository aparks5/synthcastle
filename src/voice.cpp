#include "voice.h"
#include "util.h"


Voice::Voice()
	: m_saw(SAMPLE_RATE)
	, m_tri(SAMPLE_RATE)
	, m_square(SAMPLE_RATE)
	, m_sine(SAMPLE_RATE)
	, m_lfo(SAMPLE_RATE)
	, m_saw2(SAMPLE_RATE)
	, m_tri2(SAMPLE_RATE)
	, m_square2(SAMPLE_RATE)
	, m_sine2(SAMPLE_RATE)
	, m_moogFilter(SAMPLE_RATE)
	, m_bParamChanged(false)
	, m_env1out(0.f)

{
	EnvelopeParams env(3, 0, 0, 3);
	m_env.setParams(env);
	m_moogFilter.freq(1000.f);
	m_filtFreq = 1000.f;
	m_moogFilter.q(3.f);
	m_lfo.freq(1.f);
}

void Voice::update(VoiceParams params)
{

	m_params = params;
	m_env.setParams(m_params.envParams);
	m_env.reset();
	m_lfo.freq(m_params.filtLFOFreq);

}


void Voice::modUpdate()
{
	auto lfoSamp = m_lfo();
	if (m_params.bEnableFiltLFO) {
		float filtLfo = (1 + lfoSamp * 0.5f);
		m_moogFilter.freq(m_params.filtFreq * filtLfo);

	}
	else {
		m_moogFilter.freq(m_params.filtFreq);
	}

	if (m_params.bEnablePitchLFO)
	{
		modFreq(m_params.freq + (lfoSamp * m_params.freq));
		modFreqOsc2(m_params.freq + (lfoSamp * m_params.freq));
	}
}



float Voice::apply()
{
	auto output = 0.f;

	// OSCILLATOR
	oscillate(output);
	m_lfo();

	// FILTER
	m_moogFilter.apply(&output, 1);

	// VCA 
	m_env1out = m_env.apply(1);
	m_gain.setGainf(m_env1out);
	output = m_gain.apply(output);
	Gain gain;

	// OUTPUTGAIN
	gain.setGaindB(-10);
	output = gain.apply(output);

	return output;
}

void Voice::oscillate(float& output)
{
	switch (m_params.osc) {
	case OscillatorType::SAW:
		output = m_saw();
		break;
	case OscillatorType::SINE:
		output = m_sine();
		break;
	case OscillatorType::TRIANGLE:
		output = m_tri();
		break;
	case OscillatorType::SQUARE:
		output = m_square();
		break;
	}

	if (m_params.bEnableOsc2) {
		auto osc2out = 0.f;
		switch (m_params.osc2) {
		case OscillatorType::SAW:
			osc2out = m_saw2();
			break;
		case OscillatorType::SINE:
			osc2out = m_sine2();
			break;
		case OscillatorType::TRIANGLE:
			osc2out = m_tri2();
			break;
		case OscillatorType::SQUARE:
			osc2out = m_square2();
			break;
		}
		output += 0.707 * m_osc2gain.apply(osc2out);
	}

}

void Voice::updateFreq(float freq)
{
	m_params.freq = freq;
	modFreq(freq);
	if (m_params.bEnableOsc2)
	{
		m_osc2gain.setGaindB(m_params.osc2gain);
		modFreqOsc2(freq);
	}
	m_bParamChanged = true;
}

void Voice::modFreq(float freq)
{
	m_saw.freq(freq);
	m_tri.freq(freq);
	m_square.freq(freq);
	m_sine.freq(freq);
}

void Voice::modFreqOsc2(float freq)
{
	freq = freq * semitoneToRatio(m_params.osc2coarse) * semitoneToRatio(m_params.osc2fine);
	m_saw2.freq(freq);
	m_tri2.freq(freq);
	m_square2.freq(freq);
	m_sine2.freq(freq);
}

void Voice::noteOn()
{
	m_params.bIsActive = true;
	m_env.noteOn();
	m_env.reset();
}

void Voice::noteOff()
{
	m_params.bIsActive = false;
	m_env.noteOff();
}
