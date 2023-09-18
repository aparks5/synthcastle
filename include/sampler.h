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
	float process() override;
	void update() override;

	int lookupParam(std::string str) override {
		return m_lookup[str];
	}
	std::vector<std::string> paramStrings() override
	{
		std::vector<std::string> strings;
		for (std::unordered_map<std::string, int>::iterator iter = m_lookup.begin(); iter != m_lookup.end(); ++iter) {
			auto k = iter->first;
			strings.push_back(k);
		}

		return strings;
	}

	enum SamplerParams {
		NODE_ID,
		POSITION_ID,
		POSITION,
		SPREAD, // how far the position CV can go 
		SPRAY, // s&h lfo on position
		GRAINSIZE, // how far after the start position to start looping
		GRAINSIZE_MOD, // lfo for grain size
		DISTANCE, // how long in between grains
		NUM_VOICES, // how many simultaneous grains
		PITCH_ID,
		PITCH, 
		STARTSTOP_ID,
		STARTSTOP, // 1 to start, 0 to stop
		FILENAME,
		NUM_PARAMS
	};

private:
	Envelope m_env;
	std::unordered_map<std::string, int> m_lookup = {
		{"node_id", NODE_ID},
		{"position_id", POSITION_ID},
		{"position", POSITION},
		{"spread", SPREAD},
		{"distance", DISTANCE},
		{"num_voices", NUM_VOICES},
		{"grainsize", GRAINSIZE},
		{"grainsize_mod", GRAINSIZE_MOD},
		{"spray", SPRAY},
		{"pitch_id", PITCH_ID},
		{"pitch", PITCH},
		{"filename", FILENAME},
		{"startstop_id", STARTSTOP_ID},
		{"startstop", STARTSTOP},
	};
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

