#pragma once

#include "node.h"
#include <vector>
#include <unordered_map>

class Delay : public Node 
{
public:
	Delay();
	virtual ~Delay() {};
	float process(float in) override;
	int lookupParam(std::string str) override {
		return m_lookup[str];
	}
	std::vector<std::string> paramStrings() override
	{
		std::vector<std::string> strings;
		for (std::unordered_map<std::string, int>::iterator iter = m_lookup.begin(); iter != m_lookup.end(); ++iter)
		{
			auto k = iter->first;
			strings.push_back(k);
		}

		return strings;
	}

	enum DelayParams {
		NODE_ID,
		INPUT_ID,
		DELAY_MS,
		DELAY_MS_ID,
		DRYWET_RATIO,
		FEEDBACK_RATIO,
		FEEDBACK_RATIO_ID,
		RESET,
		RESET_ID,
		NUM_PARAMS
	};


	void update(float delayMs, float feedbackRatio);
	void reset();
	void write(float val);
	float tap(float ms);
	void skip();

private:
	std::unordered_map<std::string, int> m_lookup = {
		{"node_id", NODE_ID},
		{"input_id", INPUT_ID},
		{"delay_ms", DELAY_MS},
		{"delay_ms_id", DELAY_MS_ID},
		{"drywet_ratio", DRYWET_RATIO},
		{"feedback_ratio", FEEDBACK_RATIO},
		{"feedback_ratio_id", FEEDBACK_RATIO_ID},
		{"reset", RESET},
		{"reset_id", RESET_ID}
	};

	float m_sampleRate;
	float m_maxDelaySeconds;
	float m_delayMs;
	size_t m_delaySamps;
	float m_maxDelaySamps;
	std::vector<float> m_circBuff;
	size_t m_writeIdx;
	size_t m_readIdx;
	size_t m_bufSize;
	float m_feedbackRatio;
	float m_feedbackOut;

};
