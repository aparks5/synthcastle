#ifndef COMPRESSOR_H_
#define COMPRESSOR_H_

#include "delay.h"
#include "gain.h"
#include "envelopedetector.h"

struct CompressorParams
{
	float threshdB;
	float ratio;
	float kneedB;
	float attackMs;
	float releaseMs;
	float mixRatio;
	float makeupGaindB;
};

class Compressor : public Module
{
public:
	Compressor();
	Compressor(CompressorParams params);
	void update(CompressorParams params);


private:
	Delay m_buff;
	Gain m_gain;
	float m_rms;


};

#endif