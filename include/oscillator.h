#pragma once

#include "node.h"

#include "saw.h"
#include "sine.h"
#include "square.h"
#include "triangle.h"

#include <memory>

constexpr auto FS = 44100;

class Oscillator : public Node
{
public:
	Oscillator();
	virtual ~Oscillator() {};
	float process() override;

	enum OscillatorParams {
		NODE_ID,
		FREQ_ID,
		FREQ,
		OUTPUT_ID,
		MODFREQ_ID,
		MODFREQ,
		MODDEPTH_ID,
		MODDEPTH,
		TUNING_FINE,
		TUNING_COARSE,
		WAVEFORM,
		NUM_PARAMS
	};

	enum WaveForms {
		SAW,
		SINE,
		SQUARE,
		TRIANGLE,
		NUM_WAVEFORMS
	}; // todo: noise, sample and hold

	void display() override;
	void update(); // update only once per frame
	// ideally only update if the param changes
	// do a pub sub and if notify param changed
	// then call update

private:
	int m_sampleRate;
	std::vector<std::shared_ptr<WaveForm>> m_waveforms;

};
