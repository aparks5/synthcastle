#ifndef FREEVERB_H_
#define FREEVERB_H_

#include <vector>

#include "comb.h"
#include "schroederallpass.h"

class Freeverb
{
public:
	Freeverb();
	float operator()(float in);

private:
    std::vector<SchroederAllpass> m_allpassFilters;
    std::vector<Comb> m_combFilters;

};

#endif // FREEVERB_H_