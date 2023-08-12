#pragma once

#include "node.h"
#include <unordered_map>

class Gain : public Node
{
public:
	Gain(int id);
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

	void setGainf(float gain) { m_gain = gain; };
	void setGaindB(float gaindB);

	virtual void display() override;
	virtual void link(const float* src, const float* dst) override;
	std::unordered_map<const float*, const float*> m_map;

private:
	float m_gain;
	int floatTodB(float f) const;
	float dBtoFloat(int db) const;
};
