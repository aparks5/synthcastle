#include "sampler.h"

#include <fstream>
#include <ios>
#include <vector>
#include <cfloat>

Sampler::Sampler(std::string path)
	: m_pos(0)
	, m_increment(1.f)
{
	audioFile.load(path);
	m_pos = audioFile.getNumSamplesPerChannel();
}

void Sampler::noteOn()
{
	m_pos = 0;
}

float Sampler::operator()()
{
	size_t idx = m_pos;
	m_pos += m_increment;
	if (m_pos > (audioFile.getNumSamplesPerChannel() - 1)) {
		return 0.f;
	}
	else {
		return audioFile.samples[0][idx];
	}
}

