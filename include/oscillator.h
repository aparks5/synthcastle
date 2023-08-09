#pragma once

#include "node.h"

class Oscillator : public Node
{
public:
	Oscillator(int id);
	virtual ~Oscillator() {};
	float process() override;

	enum OscillatorParams {
		FREQ,
		WAVEFORM,
		NUM_PARAMS
	};

	void display() override;
};
