#ifndef ENVELOPE_H_
#define ENVELOPE_H_

#include "math.h"
#include "constants.h"
#include "gain.h"

struct EnvelopeParams
{
	size_t attackTimeSamps;
	size_t decayTimeSamps;
	int sustainLevelf;
	size_t releaseTimeSamps;
	EnvelopeParams()
		: attackTimeSamps(0)
		, decayTimeSamps(0)
		, sustainLevelf(0)
		, releaseTimeSamps(0) {}
	EnvelopeParams(size_t attMs, size_t decMs, int susf, size_t relMs)
		: attackTimeSamps((attMs == 0) ? 0 : SAMPLE_RATE * 1.f * (attMs / 1000.f))
		, decayTimeSamps((decMs == 0 ) ? 0 : SAMPLE_RATE * 1.f * (decMs / 1000.f))
		, sustainLevelf(pow(10, susf / 20.f))
		, releaseTimeSamps((relMs == 0) ? 0 : SAMPLE_RATE * 1.f * (relMs / 1000.f)) {}
};

enum EnvelopeStage
{
	ATTACK,
	DECAY,
	SUSTAIN,
	RELEASE
};

class Envelope
{
public:
	Envelope();
	void setParams(EnvelopeParams params);
	float apply(float sample);
	void reset();

private:
	EnvelopeParams m_params;
	EnvelopeStage m_stage;
	size_t m_counter;
	Gain m_gain;


};
#endif // ENVELOPE_H_
