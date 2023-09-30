#pragma once

#include "node.h"
#include <unordered_map>

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

	enum Outputs {
		TRACK1,
		TRACK2,
		TRACK3,
		TRACK4,
		TRACK5,
		TRACK6,
		TRACK7,
		TRACK8,
		NUM_OUTPUTS
	};

	enum Params {
		NODE_ID,
		GATEMODE, // output only 1 or 0
		STEP,
		PATTERN,
		SONG,
		NUM_PARAMS
	};

private:
	static const size_t maxSteps = 16;
	static const size_t maxTracks = 8;
	static const size_t maxPatterns = 128;
	static const size_t maxSongs = 16;
	static const size_t enableOffset = NUM_PARAMS + (maxTracks * maxSteps);

	size_t step;

	float notes[8][16];
	float enabled[8][16];
};

