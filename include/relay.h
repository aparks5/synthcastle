#pragma once

#include "node.h"

class ProcessorOutput : public Node
{
public:
	ProcessorOutput(size_t index);
	virtual ~ProcessorOutput() {};

	enum Inputs {
		NUM_INPUTS
	};

	enum Params {
		NUM_PARAMS
	};
	
	enum Outputs {
		OUTPUT,
		NUM_OUTPUTS
	};

};
