#ifndef BITCRUSHER_H_
#define BITCRUSHER_H_

#include "math.h"
#include "module.h"

class Bitcrusher : public Module
{
public:
	Bitcrusher(size_t fs) : Module(fs) , m_nBits(8) {};
	void bits(size_t nbits) { m_nBits = nbits;  }

	float operator()() override { return 0.f; }
	float operator()(float in) override {
		m_output = floor(in * pow(2, (m_nBits - 1))) / (pow(2, (m_nBits - 1)));
		return m_output;
	}

private:
	size_t m_nBits;
};

#endif

