#pragma once

#include "node.h"
#include <unordered_map>

class Gain : public Node
{
public:
	Gain();
	virtual ~Gain() {};
	float process() override;

	enum GainParams {
		GAIN,
		GAINMOD,
		GAINMOD_ID,
		GAINMOD_DEPTH,
		GAIN_ID,
		LEFT_ID,
		LEFTOUT,
		RIGHT_ID,
		RIGHTOUT,
		PAN,
		PANMOD,
		PANMOD_ID,
		PANMOD_DEPTH,
		OUTPUT_ID,
		NODE_ID,
		INPUT_ID,
		INPUT,
		NUM_PARAMS
	};

	int lookupParam(std::string str) override;

	void setGainf(float gain) { m_gain = gain; };
	void setGaindB(float gaindB);

private:
	std::unordered_map<std::string, int> m_lookup = {
			{"gain", GAIN},
			{"input_id", INPUT_ID},
			{"left_id", LEFT_ID},
			{"right_id", RIGHT_ID},
			{"gainmod", GAINMOD},
			{"gainmod_depth", GAINMOD_DEPTH},
			{"pan", PAN},
			{"panmod_id", PANMOD_ID},
			{"panmod_depth", PANMOD_DEPTH},
			{"leftout", LEFTOUT},
			{"rightout", RIGHTOUT},
	};
	float m_gain;
};
