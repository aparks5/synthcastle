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
	virtual void update() override {}; // update only once per frame
	// ideally only update if the param changes
	// do a pub sub and if notify param changed
	// then call update

protected:
	float m_step;
	float m_sampleRate;
	float m_out;
};
