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
	float process() override;
	int lookupParam(std::string str) override;

	enum OscillatorParams {
		NODE_ID,
		FREQ_ID,
		FREQ,
		OUTPUT_ID,
		MODFREQ_ID,
		MODFREQ,
		MODDEPTH_ID,
		MODDEPTH,
		OUTPUT,
		TUNING_FINE,
		TUNING_COARSE,
		WAVEFORM,
		NUM_PARAMS
	};

	std::unordered_map<std::string, int> m_lookup = {
		{"node_id", NODE_ID},
		{"freq_id", FREQ_ID},
		{"freq", FREQ},
		{"output_id", OUTPUT_ID},
		{"output", OUTPUT},
		{"modfreq_id", MODFREQ_ID},
		{"modfreq", MODFREQ},
		{"moddepth_id", MODDEPTH_ID},
		{"moddepth", MODDEPTH},
		{"tuning_fine", TUNING_FINE},
		{"tuning_coarse", TUNING_COARSE},
		{"waveform", WAVEFORM}
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
