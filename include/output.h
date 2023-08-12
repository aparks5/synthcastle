#pragma once

#include "node.h"

class Output : public Node
{
public:
	Output();
	virtual ~Output() {};
	float process() override;
	void display() override;

	enum OutputParams {
		INPUT_ID,
		NODE_ID,
		NUM_PARAMS
	};
};
