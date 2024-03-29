#pragma once

class WaveForm
{
public:
	WaveForm(float sampleRate)
	: m_sampleRate(sampleRate)
	, m_freq(0.f)
	, m_modfreq(0.f)
	, m_moddepth(0.f)
	, m_step(0.f)
	, m_out(0.f)
{
}
	virtual ~WaveForm() {};
	virtual float process() = 0; // generate wave output
	virtual void update(float freq) = 0;

protected:
	float m_sampleRate;
	float m_freq;
	float m_modfreq;
	float m_moddepth;
	float m_step;
	float m_out;
};
