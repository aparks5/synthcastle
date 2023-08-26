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

	enum SeqParams {
		NODE_ID,
		TRIGIN_ID,
		TRIGIN,
		S1,
		S2,
		S3,
		S4,
		S5,
		S6,
		S7,
		S8,
		NUM_PARAMS
	};

private:
	std::unordered_map<std::string, int> m_lookup = {
			{"node_id", NODE_ID},
			{"trigin_id", TRIGIN_ID},
			{"trigin", TRIGIN},
			{"s1", S1},
			{"s2", S2},
			{"s3", S3},
			{"s4", S4},
			{"s5", S5},
			{"s6", S6},
			{"s7", S7},
			{"s8", S8}
	};
	size_t m_step;
	float m_notes[8];

};

