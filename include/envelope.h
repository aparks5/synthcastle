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

	enum EnvParams {
		NODE_ID,
		INPUT_ID,
		OUTPUT_ID,
		OUTPUT,
		TRIG_ID,
		TRIG,
		ATTACK_MS,
		DECAY_MS,
		SUSTAIN_DB,
		RELEASE_MS,
		NUM_PARAMS,
	};

	float process(float in) override;
	void reset();
	void noteOn() { m_bNoteOn = true; m_stage = EnvelopeStage::ATTACK; }
	void noteOff() { m_bNoteOn = false; m_stage = EnvelopeStage::RELEASE; }
	int lookupParam(std::string str) override;

private:
	std::unordered_map<std::string, int> m_lookup =
	{
		{"node_id", NODE_ID},
		{"input_id", INPUT_ID},
		{"output_id", OUTPUT_ID},
		{"output", OUTPUT},
		{"trigger_id", TRIG_ID},
		{"trigger", TRIG},
		{"attack_ms", ATTACK_MS},
		{"decay_ms", DECAY_MS},
		{"sustain_db", SUSTAIN_DB},
		{"release_ms", RELEASE_MS}
	};
	EnvelopeParams m_params;
	EnvelopeStage m_stage;
	float m_gain;
	bool m_bNoteOn;
	bool m_bTriggered;
};
#endif // ENVELOPE_H_
