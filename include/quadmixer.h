#pragma once

#include "node.h"
#include <unordered_map>

class QuadMixer : public Node
{
public:
	QuadMixer();
	virtual ~QuadMixer() {}
	void process() noexcept override;

	enum Inputs {
		INPUT_A,
		INPUT_B,
		INPUT_C,
		INPUT_D,
		NUM_INPUTS
	};

	enum Outputs {
		OUTPUT,
		NUM_OUTPUTS
	};

	enum Params {
		NODE_ID,
		GAIN_A,
		GAIN_B,
		GAIN_C,
		GAIN_D,
		NUM_PARAMS
	};

private:
};