#pragma once

#include "node.h"
#include <unordered_map>

// trigger outputs 1 on a button press then 0s until the next button press
class Trig : public Node
{
public:
	Trig();
	virtual ~Trig() {};
	float process() override;
	int lookupParam(std::string str) override;

	enum TriggerParams {
		NODE_ID,
		TRIG,
		TRIG_ID,
		TRIG2_ID,
		TRIG3_ID,
		TRIG4_ID,
		BPM,
		NUM_PARAMS
	};

private:
	std::unordered_map<std::string, int> m_lookup = {
			{"node_id", NODE_ID},
			{"trig_id", TRIG_ID},
			{"trig2_id", TRIG2_ID},
			{"trig3_id", TRIG3_ID},
			{"trig4_id", TRIG4_ID},
			{"trig", TRIG},
			{"bpm", BPM}
	};
	size_t m_sampsPerBeat;
	size_t m_counter;
	size_t m_cycle;
	float m_trigs[4];

};
