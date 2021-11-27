#include "voice.h"
#include "util.h"

Voice::Voice(size_t fs)
	: Module(fs)
	, m_osc(OscillatorType::SAW)
	, m_saw(fs)
	, m_tri(fs)
	, m_square(fs)
	, m_sine(fs)
	, m_lfo(fs)
	, m_pitchLfo(fs)
	, m_osc2gain(fs)
	, m_saw2(fs)
	, m_tri2(fs)
	, m_square2(fs)
	, m_sine2(fs)
	, m_gain(fs)
	, m_moogFilter(fs)
	, m_bParamChanged(false)
	, m_env1out(0.f)
{
	EnvelopeParams env(3, 250, 0, 0);
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
	m_moogFilter.q(m_params.filtQ);
	m_moogFilter.freq(m_params.filtFreq);
	m_lfo.freq(m_params.filtLFOFreq);
	m_pitchLfo.freq(m_params.pitchLFOFreq);

}


void Voice::modUpdate()
{
	if (m_params.bEnableFiltLFO) {

		auto lfoSamp = m_lfo();
		float filtLfo = (1 + lfoSamp * 0.5f);
		m_moogFilter.freq(m_params.filtFreq * filtLfo);

	}
	else {
		m_moogFilter.freq(m_params.filtFreq);
	}

	if (m_params.bEnablePitchLFO)
	{
		auto pitchLfoSamp = m_pitchLfo();
		modFreq(m_params.freq + (pitchLfoSamp * m_params.freq * m_params.pitchLFOdepth));
		modFreqOsc2(m_params.freq + (pitchLfoSamp * m_params.freq * m_params.pitchLFOdepth));
	}
}



float Voice::apply()
{
	auto output = 0.f;

	// OSCILLATOR
	oscillate(output);
	m_lfo();
	m_pitchLfo();

	// FILTER
	m_moogFilter.apply(&output, 1);

	// VCA 
	m_env1out = m_env.apply(1);
	m_gain.setGainf(m_env1out);
	output = m_gain.apply(output);
	Gain gain(SAMPLE_RATE);

	// OUTPUTGAIN
	gain.setGaindB(-5);
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

void Voice::noteOn(int noteVal)
{
	m_params.midiNote = noteVal;
	updateFreq(midiNoteToFreq(noteVal));
	m_params.bIsActive = true;
	m_env.noteOn();
	m_env.reset();
}

void Voice::noteOff(int noteVal)
{
	if (midiNote() == noteVal) {
		m_params.bIsActive = false;
		m_params.midiNote = 0;
		m_env.noteOff();
	}
}

