#pragma once

#include "node.h"
#include <unordered_map>

class Gain : public Node
{
public:
	Gain();
	virtual ~Gain() {};
	void process() noexcept override;

	enum Inputs {
		INPUT,
		GAINMOD,
		PANMOD,
		NUM_INPUTS
	};

	enum Outputs {
		OUTPUT_LEFT,
		OUTPUT_RIGHT,
		NUM_OUTPUTS
	};

	enum Params {
		INPUT_ID,
		GAINMOD_ID,
		GAIN_ID,
		LEFT_ID,
		RIGHT_ID,
		PANMOD_ID,
		OUTPUT_ID,
		NODE_ID,
		GAIN,
		GAINMOD_DEPTH,
		PAN,
		PANMOD_DEPTH,
		NUM_PARAMS
	};

	void setGainf(float gain) { m_gain = gain; };
	void setGaindB(float gaindB);

protected:
private:
	float m_gain;
};
