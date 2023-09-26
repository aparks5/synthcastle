#include "gain.h"
#include "util.h"
#include "math.h"
#include "constants.h"

Gain::Gain()
    : Node(NodeType::GAIN, "gain", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
{
	params[Gain::GAIN] = 1.f;
}

void Gain::setGaindB(float gaindB)
{
	m_gain = dBtoFloat(gaindB);
}

float Gain::process()
{
	float in = params[Gain::INPUT];

	auto g = dBtoFloat(params[Gain::GAIN]);
	in *= g * params[Gain::GAINMOD];

	auto p = params[Gain::PAN];

	if (params[Gain::PANMOD] != 0) {
		p *= params[Gain::PANMOD];
		if (params[Gain::PANMOD_DEPTH] != 0) {
			p *= params[Gain::PANMOD_DEPTH];
		}
	}

	float temp = M_PI * 0.25 * p;

    float scale = 0.7071; // ~= sqrt(2)/2
	float rightPanGain = scale * (cos(temp) + sin(temp));
	float leftPanGain = scale * (cos(temp) - sin(temp));

	params[Gain::LEFTOUT] = in * leftPanGain;
	params[Gain::RIGHTOUT] = in * rightPanGain;

	return 0;

}
