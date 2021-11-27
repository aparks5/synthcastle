#ifndef GAIN_H_
#define GAIN_H_

#include "module.h"

class Gain : public Module
{
public:
	Gain(size_t sampleRate);
	float operator()(float in) override;
	float operator()() override { return 0.f; }

	void setGaindB(float gaindB);
	void setGainf(float gainf);

private:
	int floatTodB(float f) const;
	float dBtoFloat(int db) const;
	float m_fgain;
};

#endif // GAIN_H_