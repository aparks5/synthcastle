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

enum class EnvelopeStage
{
	ATTACK,
	DECAY,
	SUSTAIN,
	RELEASE,
	OFF
};

class Envelope : public Node
{
public:
	Envelope();

	enum Inputs {
		INPUT,
		TRIG,
		NUM_INPUTS
	};

	enum Outputs {
		OUTPUT,
		NUM_OUTPUTS
	};
	enum Params {
		NODE_ID,
		INPUT_ID,
		OUTPUT_ID,
		TRIG_ID,
		ATTACK_MS,
		DECAY_MS,
		SUSTAIN_DB,
		RELEASE_MS,
		NUM_PARAMS,
	};

	void process() noexcept override;
	void reset();
	void noteOn() { m_bNoteOn = true; m_stage = EnvelopeStage::ATTACK; }
	void noteOff() { m_bNoteOn = false; m_stage = EnvelopeStage::RELEASE; }

private:
	EnvelopeParams m_params;
	EnvelopeStage m_stage;
	float m_gain;
	bool m_bNoteOn;
	bool m_bTriggered;
};
#endif // ENVELOPE_H_
