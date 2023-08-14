#pragma once

#include "node.h"

class QuadMixer : public Node
{
public:
	QuadMixer();
	virtual ~QuadMixer() {}
	float process() override;
	void display() override;
	enum QuadMixerParams {
		NODE_ID,
		INPUT_A_ID,
		INPUT_B_ID,
		INPUT_C_ID,
		INPUT_D_ID,
		INPUT_A,
		INPUT_B,
		INPUT_C,
		INPUT_D,
		NUM_PARAMS
	};
};