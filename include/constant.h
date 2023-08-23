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
	int lookupParam(std::string str) override;

	enum ConstantParams {
		NODE_ID,
		VALUE,
		NUM_PARAMS
	};

private:
	std::unordered_map<std::string, int> m_lookup = {
			{"value", VALUE}
	};

};
