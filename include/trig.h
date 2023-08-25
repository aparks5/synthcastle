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
		NUM_PARAMS
	};

private:
	std::unordered_map<std::string, int> m_lookup = {
			{"node_id", NODE_ID},
			{"trig", TRIG}
	};

};
