#pragma once

#include "node.h"
#include <unordered_map>

// while class Value is used to route input/output throughout the graph
// Constant is a UI node that only outputs whatever the UI says
class Constant : public Node
{
public:
	Constant();
	virtual ~Constant() {};
	float process() override;

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

	enum ConstantParams {
		NODE_ID,
		INPUT1_ID,
		INPUT2_ID,
		INPUT3_ID,
		INPUT4_ID,
		INPUT1,
		INPUT2,
		INPUT3,
		INPUT4,
		VALUE1,
		VALUE2,
		VALUE3,
		VALUE4,
		OUTPUT1_ID,
		OUTPUT2_ID,
		OUTPUT3_ID,
		OUTPUT4_ID,
		NUM_PARAMS
	};

private:
	std::unordered_map<std::string, int> m_lookup = {
			{"node_id", NODE_ID},
			{"input1_id", INPUT1_ID},
			{"input2_id", INPUT2_ID},
			{"input3_id", INPUT3_ID},
			{"input4_id", INPUT4_ID},
			{"input1", INPUT1},
			{"input2", INPUT2},
			{"input3", INPUT3},
			{"input4", INPUT4},
			{"value1", VALUE1},
			{"value2", VALUE2},
			{"value3", VALUE3},
			{"value4", VALUE4},
			{"output1_id", OUTPUT1_ID},
			{"output2_id", OUTPUT2_ID},
			{"output3_id", OUTPUT3_ID},
			{"output4_id", OUTPUT4_ID},
	};

};
