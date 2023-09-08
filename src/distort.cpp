#pragma once

#include "node.h"
#include <unordered_map>
#include "distort.h"

Distort::Distort()
	: Node(NodeType::DISTORT, 0.f, NUM_PARAMS)
{}

int Distort::lookupParam(std::string str)
{
	return m_lookup[str];
}

float Distort::process(float in)
{
	return 0.5*atanf(30.*in);
}
