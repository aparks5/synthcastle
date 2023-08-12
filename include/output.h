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
		NODE_ID,
		INPUT_L_ID,
		INPUT_R_ID,
		NUM_PARAMS
	};

};
