#ifndef BITCRUSHER_H_
#define BITCRUSHER_H_

#include "math.h"

class Bitcrusher
{
public:
	Bitcrusher() {};
	float operator()(float in, size_t nBits) {
		return floor(in * pow(2, (nBits - 1))) / (pow(2, (nBits - 1)));
	}
};

#endif

