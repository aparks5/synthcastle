#pragma once

#include "node.h"

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

	void setGainf(float gain) { m_gain = gain; };
	void setGaindB(float gaindB);

	virtual void display() override;

private:
	float m_gain;
	int floatTodB(float f) const;
	float dBtoFloat(int db) const;
};
