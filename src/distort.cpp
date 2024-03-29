#pragma once

#include "distort.h"
#include "util.h"

Distort::Distort()
	: Node(NodeType::PROCESSOR, "distort", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
	, m_hp(44100)
	, m_hpCutoffHz(1000.f)
{
	m_hp.update(100);
	paramMap = {
			{"node_id", NODE_ID},
			{"algorithm", ALGORITHM},
			{"preemph_cutoff", PREEMPH_CUTOFF},
			{"drive_db", DRIVE_DB},
			{"atten_db", ATTEN_DB},
			{"drywet", DRYWET},
	};

	inputMap = {
		{"input", INPUT}
	};

	outputMap = {
		{"output", OUTPUT}
	};

	initIdStrings();
}

void Distort::process() noexcept
{
	float in = inputs[INPUT];

	auto dry = in;
	auto drive_ratio = dBtoFloat(params[DRIVE_DB]);
	auto drywet = params[DRYWET];
	auto atten_ratio = dBtoFloat(params[ATTEN_DB]);
	float val = 0.f;

	if (m_hpCutoffHz != params[PREEMPH_CUTOFF]) {
		m_hpCutoffHz = params[PREEMPH_CUTOFF];
		m_hp.update(m_hpCutoffHz);
	}

	in = m_hp(in);
	in = drive_ratio * in;

	switch (static_cast<int>(params[ALGORITHM])) {
	case TANH:
	{
		val = tanhf(in);
	}
	break;
	case ATAN:
	{
		val = atanf(in);
	}
	break;
	case SIN:
	{
		val = sin(in);
	}
	break;
	case TWOSTAGE_SOFTCLIP:
	{
		if (fabs(in) > (.667)) {
			val = sgn(in);
		}
		else if ((fabs(in) >= 0.333) && (fabs(in) <= 0.667)) {
			val = sgn(in) * (3 - pow(2 - fabs(3 * in), 2)) * 0.333;
		}
		else {
			val = 2 * in;
		}
	}
	break;
	case CUBIC_SOFTCLIP:
	{
		if (fabs(in) > 0.667) {
			val = sgn(in);
		}
		else {
			val = (2.25f * in) - (1.6875 * in * in * in);
		}
	}
	break;
	case APPLY_TRIANGLE: 
	{
		val = 4 * (abs(0.25 * in + (0.25 - round(0.25 * in + 0.25)) - 0.25));
	}
	break;
	}

	val = val * atten_ratio;

	outputs[OUTPUT] = ((1 - drywet) * dry) + (drywet * val);
}
