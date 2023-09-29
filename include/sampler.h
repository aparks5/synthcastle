#pragma once

#include <memory>
#include <string>
#include <vector>
#include "node.h"
#include "envelope.h"

#include "thirdparty/AudioFile.h"

class Sampler : public Node 
{
public:
	Sampler();
	void process() noexcept override;
	void update() override;

	enum Inputs {
		PITCH,
		POSITION,
		STARTSTOP, // 1 to start, 0 to stop
		NUM_INPUTS
	};

	enum Outputs {
		OUTPUT,
		NUM_OUTPUTS
	};

	enum Params {
		NODE_ID,
		PITCH_ID,
		POSITION_ID,
		STARTSTOP_ID,
		SPREAD, // how far the position CV can go 
		SPRAY, // s&h lfo on position
		GRAINSIZE, // how far after the start position to start looping
		GRAINSIZE_MOD, // lfo for grain size
		DISTANCE, // how long in between grains
		NUM_VOICES, // how many simultaneous grains
		FILENAME,
		NUM_PARAMS
	};

private:
	Envelope m_env;
	float m_sampleRate;
	AudioFile<float> audioFile;
	size_t m_startPos;
	double m_accum;
	double m_rate;
	std::vector<float> m_samples;
	double m_increment;
	std::string m_path;
	bool m_bTriggered;
	size_t m_loopPoint;
};

