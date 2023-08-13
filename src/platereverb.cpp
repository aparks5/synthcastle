#include "platereverb.h"
#include "constants.h"

PlateReverb::PlateReverb(size_t fs)
	: Module(fs)
	, m_inputDiffusorBranch(0.f)
	, m_diffusionLFO1(SAMPLE_RATE)
	, m_diffusionLFO2(SAMPLE_RATE)
	, m_decayDiffusion1FeedbackSample(0.f)
	, m_decayDiffusion2FeedbackSample(0.f)
{
	float fb = -1.f * m_params.inputDiffusion1;

	m_inputDiffusor.push_back(
		std::make_unique<Allpass>(fs, kInputDiffusionMs[static_cast<size_t>(InputAllpasses::AP142)], fb));

	m_inputDiffusor.push_back(
		std::make_unique<Allpass>(fs, kInputDiffusionMs[static_cast<size_t>(InputAllpasses::AP107)], fb));

	fb = -1.f * m_params.inputDiffusion2;

	m_inputDiffusor.push_back(
		std::make_unique<Allpass>(fs, kInputDiffusionMs[static_cast<size_t>(InputAllpasses::AP379)], fb));

	m_inputDiffusor.push_back(
		std::make_unique<Allpass>(fs, kInputDiffusionMs[static_cast<size_t>(InputAllpasses::AP277)], fb));


	auto modfb = m_params.decayDiffusion1;
	m_diffusionLFO1.update(0.1f, 0, 0);
	m_diffusionLFO2.update(0.1f, 0, 0);

	m_modulatedDiffusor.push_back(std::make_unique<Allpass>(fs, kModulationDiffusionMs[0], modfb));
	m_modulatedDiffusor.push_back(std::make_unique<Allpass>(fs, kModulationDiffusionMs[1], modfb));

	m_delays[0] = std::make_unique<Delay>(fs, 1.f);
	m_delays[0]->update(kDelayTimesMs[0], 0.f);
	m_delays[1] = std::make_unique<Delay>(fs, 1.f);
	m_delays[1]->update(kDelayTimesMs[1], 0.f);
	m_delays[2] = std::make_unique<Delay>(fs, 1.f);
	m_delays[2]->update(kDelayTimesMs[2], 0.f);
	m_delays[3] = std::make_unique<Delay>(fs, 1.f);
	m_delays[3]->update(kDelayTimesMs[3], 0.f);

	m_lowpasses.push_back(std::make_unique<OnePoleLowpass>(fs));
	m_lowpasses[0]->update(-1.f*m_params.bandwidth);
	m_lowpasses.push_back(std::make_unique<OnePoleLowpass>(fs));
	m_lowpasses[1]->update(-1.f*m_params.hfDamping);
	m_lowpasses.push_back(std::make_unique<OnePoleLowpass>(fs));
	m_lowpasses[2]->update(-1.f*m_params.hfDamping);

	m_decayDiffusor.push_back(std::make_unique<Allpass>(fs, kDecayDiffusionTimesMs[0], m_params.decayDiffusion2));
	m_decayDiffusor.push_back(std::make_unique<Allpass>(fs, kDecayDiffusionTimesMs[1], m_params.decayDiffusion2));
	std::array<std::unique_ptr<Allpass>, 2> m_decayDiffusor;

}

void PlateReverb::reset()
{

}

float PlateReverb::operator()(float in)
{
	float out = 0.f;
	out = in * m_params.bandwidth;
	out = (*m_lowpasses[0])(out);
	out = (*m_inputDiffusor[static_cast<size_t>(InputAllpasses::AP142)])(out);
	out = (*m_inputDiffusor[static_cast<size_t>(InputAllpasses::AP107)])(out);
	out = (*m_inputDiffusor[static_cast<size_t>(InputAllpasses::AP379)])(out);
	out = (*m_inputDiffusor[static_cast<size_t>(InputAllpasses::AP277)])(out);

	m_inputDiffusorBranch = out;

	out += m_decayDiffusion2FeedbackSample;

	(*m_modulatedDiffusor[static_cast<size_t>(ModulatedAllpasses::MODAP672)]).update(22.58 + (1.f * m_diffusionLFO1.process()));
	(*m_modulatedDiffusor[static_cast<size_t>(ModulatedAllpasses::MODAP672)])(out);

	float tmpRead = (*m_delays[0])();
	(*m_delays[0]).write(out);
	out = tmpRead;


	out *= m_params.decay;

	out = (*m_decayDiffusor[0])(out);

	tmpRead = (*m_delays[1])();
	(*m_delays[1]).write(out);
	out = tmpRead;

	m_decayDiffusion1FeedbackSample = out * m_params.decay;

	(*m_modulatedDiffusor[static_cast<size_t>(ModulatedAllpasses::MODAP908)]).update(30.51 + (1.f * m_diffusionLFO1.process()));
	(*m_modulatedDiffusor[static_cast<size_t>(ModulatedAllpasses::MODAP908)])(out);

	tmpRead = (*m_delays[2])();
	(*m_delays[2]).write(out);
	out = tmpRead;

	out = (*m_lowpasses[1])(out);

	out *= m_params.decay;

	out = (*m_decayDiffusor[1])(out);

	tmpRead = (*m_delays[3])();
	(*m_delays[3]).write(out);
	out = tmpRead;

	m_decayDiffusion2FeedbackSample = out * m_params.decay;

	out += m_decayDiffusion1FeedbackSample;

	float yl = 0.f;
	float yr = 0.f;
	float accum = 0.f;

	// left wet out
	accum += 0.6 * m_modulatedDiffusor[0]->tap(8.938);
	accum += 0.6 * m_modulatedDiffusor[0]->tap(100);
	accum -= 0.6 * m_decayDiffusor[0]->tap(64.3);
	accum += 0.6 * m_delays[3]->tap(67.0);
	accum -= 0.6 * m_delays[1]->tap(67.0);
	accum -= 0.6 * m_decayDiffusor[0]->tap(6.3);
	yl = accum - 0.6 * m_delays[1]->tap(29.0);
	//// right wet out
	//return { yl, yr };
	//accum = 0.6 * node24_30[353];
	//accum += 0.6 * node24_30[3627];
	//accum -= 0.6 * node31_33[1228];
	//accum += 0.6 * node33_39[2673];
	//accum -= 0.6 * node48_54[2111];
	//accum -= 0.6 * node55_59[335];
	//yr = accum - 0.6 * node59_63[121];
	return yl;
}