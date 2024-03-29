#pragma once

#include "node.h"
#include <unordered_map>

// while class Value is used to route input/output throughout the graph
// Constant is a UI node that only outputs whatever the UI says
class Constant : public Node
{
public:
	Constant();
	virtual ~Constant() {};
	void process() noexcept override;

	enum Inputs {
		INPUT1,
		INPUT2,
		INPUT3,
		INPUT4,
		NUM_INPUTS
	};

	enum Outputs {
		OUTPUT1,
		OUTPUT2,
		OUTPUT3,
		OUTPUT4,
		NUM_OUTPUTS
	};

	enum Params {
		NODE_ID,
		VALUE1,
		VALUE2,
		VALUE3,
		VALUE4,	
		INPUT1_ID,
		INPUT2_ID,
		INPUT3_ID,
		INPUT4_ID,
		OUTPUT1_ID,
		OUTPUT2_ID,
		OUTPUT3_ID,
		OUTPUT4_ID,
		NUM_PARAMS
	};

};
