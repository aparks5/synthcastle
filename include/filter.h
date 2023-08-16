#pragma once

#include "node.h"
#include "KrajeskiMoog.h"

class Filter : public Node
{
public:
	Filter();
	virtual ~Filter() {}

	enum FiltParams {
		NODE_ID,
		INPUT_ID,
		FREQMOD_ID,
		FREQMOD,
		MODDEPTH_ID,
		MODDEPTH,
		FREQ,
		Q,
		NUM_PARAMS,
	};

	float process(float in) override;
	void display() override;

private:
	KrajeskiMoog m_moogFilter;
	float m_freq;
	float m_q;

};
