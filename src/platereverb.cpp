#include "platereverb.h"

PlateReverb::PlateReverb()
{
	float fb = 0.5f;

	m_inputDiffusor[static_cast<size_t>(InputAllpasses::AP142)] =
		std::make_unique<Allpass>(kInputDiffusionMs[static_cast<size_t>(InputAllpasses::AP142)], fb);

	m_inputDiffusor[static_cast<size_t>(InputAllpasses::AP107)] =
		std::make_unique<Allpass>(kInputDiffusionMs[static_cast<size_t>(InputAllpasses::AP107)], fb);

	m_inputDiffusor[static_cast<size_t>(InputAllpasses::AP379)] =
		std::make_unique<Allpass>(kInputDiffusionMs[static_cast<size_t>(InputAllpasses::AP379)], fb);

	m_inputDiffusor[static_cast<size_t>(InputAllpasses::AP277)] =
		std::make_unique<Allpass>(kInputDiffusionMs[static_cast<size_t>(InputAllpasses::AP277)], fb);
}


void PlateReverb::reset()
{

}


std::pair<float, float> PlateReverb::operator()(float in)
{
	float out = 0.f;
	out = in * m_params.bandwidth;
	out = (*m_inputDiffusor[static_cast<size_t>(InputAllpasses::AP142)])(out);
	out = (*m_inputDiffusor[static_cast<size_t>(InputAllpasses::AP107)])(out);
	out = (*m_inputDiffusor[static_cast<size_t>(InputAllpasses::AP379)])(out);
	out = (*m_inputDiffusor[static_cast<size_t>(InputAllpasses::AP277)])(out);


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

	return { yl, yr };
}