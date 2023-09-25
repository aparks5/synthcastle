#pragma once

#include "node.h"
#include <unordered_map>

#define NUM_TRIGS (1)

// trigger outputs 1 on a button press then 0s until the next button press
class Trig : public Node
{
public:
	Trig();
	virtual ~Trig() {};
	float process() override;
	int lookupParam(std::string str) override;
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

	enum TriggerParams {
		NODE_ID,
		TRIG,
		PROGRESS,
		PROGRESS_DIR,
		START,
		STOP,
		TRIGOUT_ID,
		TRIGOUT,
		BPM,
		NUM_PARAMS
	};

private:
	std::unordered_map<std::string, int> m_lookup = {
			{"node_id", NODE_ID},
			{"trigout_id", TRIGOUT_ID},
			{"trigout", TRIGOUT},
			{"progress", PROGRESS},
			{"progress_dir", PROGRESS_DIR},
			{"trig", TRIG},
			{"bpm", BPM},
			{"start", START},
			{"stop", STOP}
	};
	size_t m_sampsPerBeat;
	size_t m_counter;
	float m_cache;

};
