#ifndef PLATE_H_
#define PLATE_H_

#include "allpass.h"
#include "delay.h"
#include "onepolelowpass.h"

#include <array>
#include <memory>

// based on Dattoro 1997, Fig. 1 "in the style of Griesinger"

struct PlateParams
{

	float decay;
	float decayDiffusion1;
	float decayDiffusion2;
	float inputDiffusion1;
	float inputDiffusion2;
	float bandwidth;
	float hfDamping;
	PlateParams()
		: decay(0.5)
		, decayDiffusion1(0.7)
		, decayDiffusion2(0.5)
		, inputDiffusion1(0.75)
		, inputDiffusion2(0.625)
		, bandwidth(0.9995)
		, hfDamping(0.0005)
	{}
};

enum class PlateDelays
{
	Z4453,
	Z3720,
	Z4217,
	Z3163
};

enum class ModulatedAllpasses
{
	MODAP672,
	MODAP908
};

enum class InputAllpasses
{
	AP142,
	AP107,
	AP379,
	AP277
};

class PlateReverb
{
public:
	PlateReverb();
	std::pair<float,float> operator()(float in);
	void update(PlateParams params) { m_params = params; }
    void reset();

private:
	PlateParams m_params;
	std::array<float, 4> kInputDiffusionMs = { 4.7713,3.5953,12.2734,9.30748 };
	std::array<std::unique_ptr<Allpass>, 4> m_inputDiffusor;

	const size_t kMaxDelayModExcursionSamps = 16;
	const std::array<float, 2> kModulationDiffusionMs = { 15.238, 20.589 };
	std::array<std::unique_ptr<Allpass>, 2> m_modulatedDiffusor;
	std::array<std::unique_ptr<Allpass>, 2> m_decayDiffusor;
	std::array<std::unique_ptr<Delay>, 4> m_delay;

	const std::array<float, 4> kDelayTimesSamples = { 100.975, 84.3537, 95.6235, 71.7234 };
	std::array<std::unique_ptr<OnePoleLowpass>, 3> m_lowpass;

	float m_decayDiffusion1FeedbackSample;
	float m_decayDiffusion2FeedbackSample;

};

#endif // PLATE_H_
