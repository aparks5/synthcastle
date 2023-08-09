#pragma once

#include "node.h"

class Output : public Node
{
public:
	Output(int id);
	virtual ~Output() {};
	float process() override;
	void display() override;
};
