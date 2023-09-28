#pragma once

#include "node.h"
#include <unordered_map>

// 8 step sequencer
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
		TRIGOUT,
		NUM_OUTPUTS
	};

	enum Params {
		NODE_ID,
		TRIGIN_ID,
		RESET_ID,
		TRIGOUT_ID,
		GATEMODE, // output only 1 or 0
		STEP,
		S1,
		S2,
		S3,
		S4,
		S5,
		S6,
		S7,
		S8,
		S1_ON,
		S2_ON,
		S3_ON,
		S4_ON,
		S5_ON,
		S6_ON,
		S7_ON,
		S8_ON,
		NUM_PARAMS
	};

private:
	size_t m_step;
	float m_lastEnabledVal;
	size_t m_peek;
	float m_notes[8];
	float m_enabled[8];

};

