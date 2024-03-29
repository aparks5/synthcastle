#pragma once

#include "node.h"

class ProcessorInput : public Node
{
public:
	ProcessorInput();
	ProcessorInput(float val);
	virtual ~ProcessorInput() {};

	enum Inputs {
		NUM_INPUTS
	};

	enum Outputs {
		OUTPUT,
		NUM_OUTPUTS
	};

	enum Params {
		NUM_PARAMS
	};

};