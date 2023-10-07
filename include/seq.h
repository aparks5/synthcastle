#pragma once

#include "node.h"
#include <unordered_map>
#include <random>

class Seq : public Node
{
public:
	Seq();
	virtual ~Seq() {};
	void process() noexcept override;

	enum Inputs {
		TRIGIN,
		RESET,
		NUM_INPUTS
	};

	struct StepParams
	{
		float enabled;
		float gate;
		float note;
		float length;
		float lengthCounter;
		float probability;
		float retrig;
		StepParams()
			: enabled(0)
			, gate(0)
			, note(0)
			, length(0)
			, lengthCounter(0)
			, probability(0)
			, retrig(0)
		{}
	};

	struct Pattern
	{
		StepParams sequence[4][16];
	};

	enum Outputs {
		CV1,
		GATE1,
		CV2,
		GATE2,
		CV3,
		GATE3,
		CV4,
		GATE4,
		NUM_OUTPUTS
	};

	enum Params {
		NODE_ID,
		DISPLAY_STEP,
		DISPLAY_NOTE,
		STEP,
		TRACK,
		RECALL,
		NOTE,
		LENGTH,
		PROBABILITY,
		RETRIG,
		PATTERN,
		SONG,
		NUM_PARAMS
	};

private:
	void update();
	float randomProbability();
	static const size_t maxSteps = 16;
	static const size_t maxTracks = 4;
	static const size_t maxPatterns = 64;
	static const size_t enableOffset = NUM_PARAMS + (maxTracks * maxSteps);

	int step;

	StepParams sequence[4][16];
	float m_noteCache[4]; // allows notes to be sustained
	Pattern pattern[64];

	std::random_device randomDevice; // obtain a random number from hardware
	std::mt19937 randomGenerator; // seed the generator
	std::uniform_int_distribution<> random; // define the range
};

