#pragma once

#include "hilbert.h"
#include "node.h"

class FrequencyShifter : public Node
{
public:
	FrequencyShifter();
	virtual ~FrequencyShifter() {}
	void process() noexcept override;

	enum Inputs {
		INPUT,
		MODFREQ,
		NUM_INPUTS
	};

	enum Outputs {
		OUTPUT,
		NUM_OUTPUTS
	};

	enum Params {
		NODE_ID,
		FREQ,
		MODDEPTH,
		DRYWET,
		NUM_PARAMS
	};

private:
	HilbertTransform hilbert;
	size_t m_fs;
	
};