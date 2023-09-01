#ifndef SAMPLER_H_
#define SAMPLER_H_

#include <memory>
#include <string>
#include <vector>
#include "node.h"

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
		INPUT_ID,
		INPUT, // input governs sample position
		STARTSTOP_ID,
		STARTSTOP, // 1 to start, 0 to stop
		FILENAME,
		NUM_PARAMS
	};

private:
	std::unordered_map<std::string, int> m_lookup = {
		{"node_id", NODE_ID},
		{"input_id", INPUT_ID},
		{"input", INPUT},
		{"filename", FILENAME},
		{"startstop_id", STARTSTOP_ID},
		{"startstop", STARTSTOP},
	};
	float m_sampleRate;
	AudioFile<float> audioFile;
	size_t m_pos;
	double m_accum;
	double m_rate;
	std::vector<float> m_samples;
	double m_increment;
	std::string m_path;
	bool m_bDone = false;
};


#endif // SAMPLER_H_