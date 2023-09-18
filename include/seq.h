#pragma once

#include "node.h"
#include <unordered_map>

// 8 step sequencer
class Seq : public Node
{
public:
	Seq();
	virtual ~Seq() {};
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

	enum SeqParams {
		NODE_ID,
		GATEMODE, // output only 1 or 0
		TRIGIN_ID,
		TRIGIN,
		RESET_ID,
		RESET,
		STEP,
		S1,
		S2,
		S3,
		S4,
		S5,
		S6,
		S7,
		S8,
		S1_ON,
		S2_ON,
		S3_ON,
		S4_ON,
		S5_ON,
		S6_ON,
		S7_ON,
		S8_ON,
		NUM_PARAMS
	};

private:
	std::unordered_map<std::string, int> m_lookup = {
			{"node_id", NODE_ID},
			{"step", STEP},
			{"trigin_id", TRIGIN_ID},
			{"trigin", TRIGIN},
			{"reset", RESET},
			{"gatemode", GATEMODE},
			{"s1", S1},
			{"s2", S2},
			{"s3", S3},
			{"s4", S4},
			{"s5", S5},
			{"s6", S6},
			{"s7", S7},
			{"s8", S8},
			{"enable_s1", S1_ON},
			{"enable_s2", S2_ON},
			{"enable_s3", S3_ON},
			{"enable_s4", S4_ON},
			{"enable_s5", S5_ON},
			{"enable_s6", S6_ON},
			{"enable_s7", S7_ON},
			{"enable_s8", S8_ON}
	};
	size_t m_step;
	float m_lastEnabledVal;
	size_t m_peek;
	float m_notes[8];
	float m_enabled[8];

};

