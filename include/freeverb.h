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

	enum Inputs {
		NUM_INPUTS
	};

	enum Outputs {
		NUM_OUTPUTS
	};

	enum FreeverbParams {
		NODE_ID,
		INPUT_ID,
		WETDRY,
		NUM_PARAMS,
	};

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