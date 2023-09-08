#pragma once

#include "node.h"
#include <unordered_map>

class Gain : public Node
{
public:
	Gain();
	virtual ~Gain() {};
	float process(float in) override;

	enum GainParams {
		GAIN,
		GAINMOD,
		GAINMOD_ID,
		GAIN_ID,
		OUTPUT_ID,
		NODE_ID,
		INPUT_ID,
		NUM_PARAMS
	};

	int lookupParam(std::string str) override;

	void setGainf(float gain) { m_gain = gain; };
	void setGaindB(float gaindB);

private:
	std::unordered_map<std::string, int> m_lookup = {
			{"gain", GAIN},
			{"gainmod", GAINMOD},
	};
	float m_gain;
};
