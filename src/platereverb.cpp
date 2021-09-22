#include "platereverb.h"

PlateReverb::PlateReverb()
{
	std::unique_ptr<Allpass> ap142 = std::make_unique<Allpass>();
	std::unique_ptr<Allpass> ap107 = std::make_unique<Allpass>();
	std::unique_ptr<Allpass> ap379 = std::make_unique<Allpass>();
	std::unique_ptr<Allpass> ap277 = std::make_unique<Allpass>();
	m_inputDiffusor[InputAllpasses::AP142] = ap142;
	m_inputDiffusor[InputAllpasses::AP107] = ap107;
	m_inputDiffusor[InputAllpasses::AP379] = ap379;
	m_inputDiffusor[InputAllpasses::AP277] = ap277;
}


void PlateReverb::reset()
{

}


std::pair<float, float> PlateReverb::operator()(float in)
{
	float out = 0.f;
	out = in * m_params.bandwidth;
	out = m_inputDiffusor[AP142];


	float yl = 0.f;
	float yr = 0.f;
	float accum = 0.f;
	/*
	// left wet out
	accum += 0.6 * node48_54[1266]
	accum += 0.6 * node48_54[2974];
	accum -= 0.6 * node55_59[1913];
	accum += 0.6 * node59_63[1996];
	accum -= 0.6 * node24_30[1990];
	accum -= 0.6 * node31_33[187];
	yl = accum - 0.6 * node33_39[1066];
	// right wet out
	accum = 0.6 * node24_30[353];
	accum += 0.6 * node24_30[3627];
	accum -= 0.6 * node31_33[1228];
	accum += 0.6 * node33_39[2673];
	accum -= 0.6 * node48_54[2111];
	accum -= 0.6 * node55_59[335];
	yr = accum - 0.6 * node59_63[121];
	*/

}