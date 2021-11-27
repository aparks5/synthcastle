#ifndef BITCRUSHER_H_
#define BITCRUSHER_H_

#include "math.h"
#include "module.h"

class Bitcrusher : public Module
{
public:
	Bitcrusher(size_t fs) : Module(fs) {};
	float operator()(float in, size_t nBits) {
		return floor(in * pow(2, (nBits - 1))) / (pow(2, (nBits - 1)));
	}
};

#endif

