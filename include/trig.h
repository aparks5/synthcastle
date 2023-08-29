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

	enum TriggerParams {
		NODE_ID,
		TRIG,
		START,
		STOP,
		NUMTRIGS,
		TRIG1_ID,
		//TRIG2_ID,
		//TRIG3_ID,
		//TRIG4_ID,
		//TRIG5_ID,
		//TRIG6_ID,
		//TRIG7_ID,
		//TRIG8_ID,
		//TRIG9_ID,
		//TRIG10_ID,
		//TRIG11_ID,
		//TRIG12_ID,
		//TRIG13_ID,
		//TRIG14_ID,
		//TRIG15_ID,
		//TRIG16_ID,
		BPM,
		NUM_PARAMS
	};

private:
	std::unordered_map<std::string, int> m_lookup = {
			{"node_id", NODE_ID},
			{"trig1_id", TRIG1_ID},
			//{"trig2_id", TRIG2_ID},
			//{"trig3_id", TRIG3_ID},
			//{"trig4_id", TRIG4_ID},
			//{"trig5_id", TRIG5_ID},
			//{"trig6_id", TRIG6_ID},
			//{"trig7_id", TRIG7_ID},
			//{"trig8_id", TRIG8_ID},
			//{"trig9_id", TRIG9_ID},
			//{"trig10_id", TRIG10_ID},
			//{"trig11_id", TRIG11_ID},
			//{"trig12_id", TRIG12_ID},
			//{"trig13_id", TRIG13_ID},
			//{"trig14_id", TRIG14_ID},
			//{"trig15_id", TRIG15_ID},
			//{"trig16_id", TRIG16_ID},
			{"trig", TRIG},
			{"numtrigs", NUMTRIGS},
			{"bpm", BPM},
			{"start", START},
			{"stop", STOP}
	};
	size_t m_sampsPerBeat;
	size_t m_counter;
	size_t m_cycle;
	float m_cache;
	float m_trigs[NUM_TRIGS];

};
