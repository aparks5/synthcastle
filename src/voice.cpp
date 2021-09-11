#include "..\include\voice.h"
#include "voice.h"

Voice::Voice()
	: durationCounter(0)
	, m_freq(0.)
	, m_osc(OscillatorType::SINE)
	, m_saw(SAMPLE_RATE)
	, m_saw2(SAMPLE_RATE)
	, m_tri(SAMPLE_RATE)
	, m_square(SAMPLE_RATE)
	, m_sine(SAMPLE_RATE)
	, m_lfo(SAMPLE_RATE)
	, m_bEnableFilterLFO(false)
	, m_bEnablePitchLFO(false)
	, m_moogFilter(SAMPLE_RATE)
	, m_filtFreq(0.f)
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

void Voice::update()
{
	// UPDATE PARAMS ONCE PER BLOCK
	auto samplesPerDuration = SAMPLE_RATE;
	auto lfo1depth = 0.1f;
	auto lfoSamp = m_lfo();
	if (m_bEnableFilterLFO) {
		float filtLfo = (1 + lfoSamp * 0.5f);
		m_moogFilter.freq(m_filtFreq * filtLfo);

	}
	else {
		m_moogFilter.freq(m_filtFreq);
	}

	if (m_bEnablePitchLFO)
	{
		modFreq(m_freq + (lfoSamp *  m_freq));
	}

	processUpdates();
}

float Voice::apply()
{
	auto output = 0.f;
	m_metronome.tick();

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
	switch (m_osc) {
	case OscillatorType::SAW:
		output = (m_saw() + m_saw2()) * 0.5f;
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
}



void Voice::updateFreq(float freq)
{
	m_freq = freq;
	modFreq(freq);
	m_bParamChanged = true;
}

void Voice::modFreq(float freq)
{
	m_saw.freq(freq);
	m_saw2.freq(freq * 1.3348);
	m_tri.freq(freq);
	m_square.freq(freq);
	m_sine.freq(freq);
}

void Voice::updateGain(int gaindB)
{
	m_gain.setGaindB(gaindB);
	m_bParamChanged = true;
}

void Voice::updateOsc(OscillatorType osc)
{
	m_osc = osc;
	m_bParamChanged = true;
}

void Voice::updateEnv(EnvelopeParams params)
{
	m_envParams = params;
	m_bParamChanged = true;
}

void Voice::enableFiltLFO()
{
	m_bEnableFilterLFO = true;
}

void Voice::disableFiltLFO()
{
	m_bEnableFilterLFO = false;
}

void Voice::noteOn()
{
	m_env.noteOn();
	m_env.reset();
}

void Voice::noteOff()
{
	m_env.noteOff();
}

void Voice::updateLfoRate(double freq)
{
	m_lfo.freq(freq);
	m_bParamChanged = true;
}

void Voice::updateFilterCutoff(double freq)
{
	m_moogFilter.freq(freq);
	m_filtFreq = freq;
}

void Voice::updateFilterResonance(double q)
{
	m_moogFilter.q(q);
}

void Voice::processUpdates()
{

	if (m_bParamChanged) {
		m_env.setParams(m_envParams);

		m_bParamChanged = false;
	}
}


