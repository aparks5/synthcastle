#pragma once

#include "node.h"
#include <unordered_map>
#include "KrajeskiMoog.h"

class Filter : public Node
{
public:
	Filter();
	virtual ~Filter() {}

	enum Inputs {
		NUM_INPUTS,
	};

	enum Outputs {
		NUM_OUTPUTS,
	};

	enum Params {
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

	void process() noexcept override;

private:
	KrajeskiMoog m_moogFilter;
	float m_freq;
	float m_q;

};
