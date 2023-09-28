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
	, m_osc2gain()
	, m_saw2(fs)
	, m_tri2(fs)
	, m_square2(fs)
	, m_sine2(fs)
	, m_gain()
	, m_moogFilter(fs)
	, m_bParamChanged(false)
	, m_env1out(0.f)
{
	EnvelopeParams env(3, 250, 0, 0);
	m_moogFilter.freq(1000.f);
	m_filtFreq = 1000.f;
	m_moogFilter.q(3.f);
	m_lfo.update(1.f);
}

void Voice::update(VoiceParams params)
{

	m_params = params;
	m_moogFilter.q(m_params.filtQ);
	m_moogFilter.freq(m_params.filtFreq);
// todo	m_lfo.params[0] = m_params.filtLFOFreq;
//	m_pitchLfo.params[Oscillator::FREQ] = m_params.pitchLFOFreq;

}


void Voice::modUpdate()
{
	if (m_params.bEnableFiltLFO) {

		auto lfoSamp = m_lfo.process();
		float filtLfo = (1 + lfoSamp * 0.5f);
		m_moogFilter.freq(m_params.filtFreq * filtLfo);

	}
	else {
		m_moogFilter.freq(m_params.filtFreq);
	}

	if (m_params.bEnablePitchLFO)
	{
		auto pitchLfoSamp = m_pitchLfo.process();
		modFreq(m_params.freq + (pitchLfoSamp * m_params.freq * m_params.pitchLFOdepth));
		modFreqOsc2(m_params.freq + (pitchLfoSamp * m_params.freq * m_params.pitchLFOdepth));
	}
}



float Voice::operator()()
{
	auto output = 0.f;

	// OSCILLATOR
	oscillate(output);
	m_lfo.process();
	m_pitchLfo.process();

	// FILTER
	m_moogFilter.apply(&output, 1);

	// VCA 
	m_env.inputs[Envelope::INPUT] = output;
	m_env.process();
	output = m_env.outputs[Envelope::OUTPUT];
	Gain gain;

	// OUTPUTGAIN
	gain.setGaindB(-5);
	//output = gain.process(output);

	return output;
}

void Voice::oscillate(float& output)
{
	switch (m_params.osc) {
	case OscillatorType::SAW:
		output = m_saw.process();
		break;
	case OscillatorType::SINE:
		output = m_sine.process();
		break;
	case OscillatorType::TRIANGLE:
		output = m_tri.process();
		break;
	case OscillatorType::SQUARE:
		output = m_square.process();
		break;
	}

	if (m_params.bEnableOsc2) {
		auto osc2out = 0.f;
		switch (m_params.osc2) {
		case OscillatorType::SAW:
			osc2out = m_saw2.process();
			break;
		case OscillatorType::SINE:
			osc2out = m_sine2.process();
			break;
		case OscillatorType::TRIANGLE:
			osc2out = m_tri2.process();
			break;
		case OscillatorType::SQUARE:
			osc2out = m_square2.process();
			break;
		}
		//output += 0.707 * m_osc2gain.process(osc2out);
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
	(void)freq;
	//m_saw.params[Oscillator::FREQ] = freq;
	//m_tri.params[Oscillator::FREQ] = freq;
	//m_square.params[Oscillator::FREQ] = freq;
	//m_sine.params[Oscillator::FREQ] = freq;
}

void Voice::modFreqOsc2(float freq)
{
	(void)freq;
	//freq = freq * semitoneToRatio(m_params.osc2coarse) * semitoneToRatio(m_params.osc2fine);
	//m_saw2.params[Oscillator::FREQ] = freq;
	//m_tri2.params[Oscillator::FREQ] = freq;
	//m_square2.params[Oscillator::FREQ] = freq;
	//m_sine2.params[Oscillator::FREQ] = freq;
}

void Voice::noteOn(int noteVal)
{
	m_params.midiNote = noteVal;
	updateFreq(midiNoteToFreq(noteVal));
	m_params.bIsActive = true;
}

void Voice::noteOff(int noteVal)
{
	if (midiNote() == noteVal) {
		m_params.bIsActive = false;
		m_params.midiNote = 0;
	}
}

