#pragma once

#include "node.h"

class Gain : public Node
{
public:
	Gain(int id);
	virtual ~Gain() {};
	float process() override;
	enum GainParams {
		GAIN,
		NUM_PARAMS
	};

	virtual void display() override;
};
