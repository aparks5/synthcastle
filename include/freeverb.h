#ifndef FREEVERB_H_
#define FREEVERB_H_


#include "node.h"
#include "comb.h"
#include "allpass.h"
#include <vector>

class Freeverb : public Node 
{
public:
	Freeverb();
	virtual ~Freeverb() {}
	float process(float in) override;

	enum FreeverbParams {
		NODE_ID,
		INPUT_ID,
		WETDRY,
		NUM_PARAMS,
	};

	int lookupParam(std::string str) override {
		return m_lookup[str];
	}
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
    std::vector<Allpass> m_allpassFilters;
    std::vector<Comb> m_combFilters;
	size_t m_sampleRate;

	std::unordered_map<std::string, int> m_lookup =
	{
		{"node_id", NODE_ID},
		{"input_id", INPUT_ID},
		{"wetdry", WETDRY}

	};

};

#endif // FREEVERB_H_