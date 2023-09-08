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
		DRIVE_DB, // amount of gain to feed into waveshaper
		ATTEN_DB, // post distortion attenuation in db
		PREEMPH_CUTOFF, // pre-emphasis filter cutoff
		ALGORITHM,
		NUM_PARAMS
	};
	
	enum DistortAlgos {
		TANH,
		ATAN,
		SIN
	};

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


private:
	std::unordered_map<std::string, int> m_lookup = {
			{"node_id", NODE_ID},
			{"algorithm", ALGORITHM},
			{"preemph_cutoff", ALGORITHM},
			{"drive_db", DRIVE_DB},
			{"atten_db", ATTEN_DB},
	};

};
