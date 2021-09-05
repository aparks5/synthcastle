#ifndef ENVELOPE_H_
#define ENVELOPE_H_

#include "math.h"
#include "constants.h"
#include "gain.h"

struct EnvelopeParams
{
	size_t attackTimeSamps;
	size_t decayTimeSamps;
	int sustainLeveldB;
	size_t releaseTimeSamps;
	EnvelopeParams()
		: attackTimeSamps(0)
		, decayTimeSamps(0)
		, sustainLeveldB(0)
		, releaseTimeSamps(0) {}
	EnvelopeParams(size_t attMs, size_t decMs, int susdB, size_t relMs)
		: attackTimeSamps((attMs == 0) ? 0 : SAMPLE_RATE * 1.f * (attMs / 1000.f))
		, decayTimeSamps((decMs == 0 ) ? 0 : SAMPLE_RATE * 1.f * (decMs / 1000.f))
		, sustainLeveldB(susdB)
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
	float apply(size_t numSamples);
	void reset();

private:
	EnvelopeParams m_params;
	EnvelopeStage m_stage;
	size_t m_counter;
	float m_gain;
};
#endif // ENVELOPE_H_
