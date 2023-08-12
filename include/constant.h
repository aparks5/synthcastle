#pragma once

#include "node.h"

// while class Value is used to route input/output throughout the graph
// Constant is a UI node that only outputs whatever the UI says
class Constant : public Node
{
public:
	Constant();
	virtual ~Constant() {};
	void display() override;
	float process() override;

	enum ConstantParams {
		NODE_ID,
		NUM_PARAMS
	};

};
