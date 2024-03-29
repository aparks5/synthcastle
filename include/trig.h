#pragma once

#include "node.h"
#include <unordered_map>

// trigger outputs 1 on a button press then 0s until the next button press
class Trig : public Node
{
public:
	Trig();
	virtual ~Trig() {};
	void process() noexcept override;

	enum Inputs {
		NUM_INPUTS
	};

	enum Outputs {
		TRIGOUT,
		TRIG8,
		TRIG16,
		NUM_OUTPUTS
	};

	enum Params {
		NODE_ID,
		TRIG,
		PROGRESS,
		PROGRESS_DIR,
		START,
		STOP,
		BPM,
		NUM_PARAMS
	};

private:
	size_t m_sampsPerBeat;
	size_t m_sampsPer8th;
	size_t m_sampsPer16th;
	volatile size_t m_counter;
	volatile size_t m_counter8;
	volatile size_t m_counter16;
	size_t m_bpm;

};
