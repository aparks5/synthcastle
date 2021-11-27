#ifndef FREEVERB_H_
#define FREEVERB_H_

#include <vector>

#include "module.h"
#include "comb.h"
#include "allpass.h"

class Freeverb : public Module
{
public:
	Freeverb(size_t fs);
	float operator()(float in) override;
	float operator()() override { return 0.f; }

private:
    std::vector<Allpass> m_allpassFilters;
    std::vector<Comb> m_combFilters;

};

#endif // FREEVERB_H_