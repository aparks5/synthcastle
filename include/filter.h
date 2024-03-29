#pragma once

#include "node.h"
#include <unordered_map>
#include "KrajeskiMoog.h"
#include "diodeladder.h"

class Filter : public Node
{
public:
	Filter();
	virtual ~Filter() {}

	enum Inputs {
		INPUT,
		FREQMOD,
		MODDEPTH,
		NUM_INPUTS,
	};

	enum Outputs {
		OUTPUT,
		NUM_OUTPUTS,
	};

	enum Params {
		NODE_ID,
		FILTER_TYPE,
		FREQ,
		Q,
		NUM_PARAMS,
	};

	enum FilterType {
		MOOG,
		DIODE
	};

	void process() noexcept override;

private:
	KrajeskiMoog moog;
	DiodeLadder diode;
	FilterType filterType;
	float m_freq;
	float m_q;

};
