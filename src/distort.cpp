#pragma once

#include "distort.h"
#include "util.h"

Distort::Distort()
	: Node(NodeType::DISTORT, 0.f, NUM_PARAMS)
{}

int Distort::lookupParam(std::string str)
{
	return m_lookup[str];
}

float Distort::process(float in)
{
	auto drive_ratio = dBtoFloat(params[DRIVE_DB]);
	auto atten_ratio = dBtoFloat(params[ATTEN_DB]);

	float val = 0;
	switch (static_cast<int>(params[ALGORITHM])) {
	case TANH:
	{
		val = atten_ratio * tanhf(drive_ratio * in);
	}
	break;
	case ATAN:
	{
		val = atten_ratio * atanf(drive_ratio * in);
	}
	break;
	case SIN:
	{
		val = atten_ratio * sin(drive_ratio * in);
	}
	break;
	}

	return val;
}
