#pragma once

#include "node.h"

#include "saw.h"
#include "sine.h"
#include "square.h"
#include "triangle.h"
#include "sampleandhold.h"
#include "noise.h"

#include <unordered_map>
#include <memory>

constexpr auto FS = 44100;

class Oscillator : public Node
{
public:
	Oscillator();
	virtual ~Oscillator() {};
	void process() noexcept override;

	enum Inputs {
		MODFREQ,
		MODDEPTH,
		NUM_INPUTS
	};

	enum Outputs {
		OUTPUT,
		NUM_OUTPUTS
	};

	enum Params {
		NODE_ID,
		FREQ_ID,
		MODFREQ_ID,
		MODDEPTH_ID,
		OUTPUT_ID,
		FREQ,
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
		SAMPLE_AND_HOLD,
		NOISE,
		NUM_WAVEFORMS
	};

	void update() override; // update only once per frame
	// ideally only update if the param changes
	// do a pub sub and if notify param changed
	// then call update

private:
	int m_sampleRate;
	std::vector<std::shared_ptr<WaveForm>> m_waveforms;

};
