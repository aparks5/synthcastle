#pragma once

#include "node.h"
#include <unordered_map>

class Distort : public Node
{
public:
	Distort();
	virtual ~Distort() {};
	int lookupParam(std::string str) override;
	float process(float in) override;

	enum DistortParams {
		NODE_ID,
		INPUT_ID,
		DRIVE_DB,
		PREEMPH_CUTOFF, // pre-emphasis filter cutoff
		ALGORITHM,
		NUM_PARAMS
	};

private:
	std::unordered_map<std::string, int> m_lookup = {
			{"node_id", NODE_ID},
			{"algorithm", ALGORITHM},
			{"preemph_cutoff", ALGORITHM},
			{"drive_db", DRIVE_DB},
	};

};
