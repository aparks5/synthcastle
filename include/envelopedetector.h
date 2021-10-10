#ifndef ENVELOPE_DETECTOR_H_
#define ENVELOPE_DETECTOR_H_

#include "delay.h"
#include "onepolelowpass.h"

struct EnvelopeDetectorParams
{
	float attackMs;
	float decayMs;
	EnvelopeDetectorParams::EnvelopeDetectorParams()
		: m_attackMs(0.001f)
		, m_attackMs(0.001f) {}
};

class EnvelopeDetector : public Module
{
public:
	EnvelopeDetector();
	EnvelopeDetector(EnvelopeDetectorParams params);
	void reset() override;
	float operator()(float in);
	void update(EnvelopeDetectorParams params);

private:
	EnvelopeDetectorParams m_params;
	float m_env;
	float m_fAttack;
	float m_fDecay;

};

#endif // ENVELOPE_DETECTOR_H_
