#include "sampler.h"

#include <fstream>
#include <ios>
#include <vector>
#include <cfloat>
#include "module.h"
#include "util.h"

Sampler::Sampler(size_t sampleRate, std::string path)
	: Module(sampleRate)
	, m_pos(0)
	, m_accum(0)
	, m_increment(1.)
	, m_rate(1.)
{
	// add loop mode for wavetables
	audioFile.load(path);
	m_pos = audioFile.getNumSamplesPerChannel();
	m_accum = audioFile.getNumSamplesPerChannel();
}

void Sampler::noteOn(size_t noteVal)
{
	(void)noteVal;
	m_pos = 0;
	m_accum = 0;
	m_rate = midiNoteToFreq(noteVal) / 440.;
}

void Sampler::noteOff(size_t noteVal)
{
	(void)noteVal;
}

float Sampler::operator()()
{
	// playing back at 44.1kHz, advance only 1 sample each time
	m_accum += m_rate*m_increment;
		// linearInterpolate()
	if (m_accum > (audioFile.getNumSamplesPerChannel() - 1)) {
		return 0.f;
	}
	else {
		int nearest = (int)m_accum;
		float remainder = fmodf(m_accum, nearest);

		if (nearest >= 1) {
			return linearInterpolate(audioFile.samples[0][nearest - 1], audioFile.samples[0][nearest], remainder);
		} 
		else {
			return audioFile.samples[0][0];
		}
	}
}

