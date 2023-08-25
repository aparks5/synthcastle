#include "trig.h"

Trig::Trig()
	: Node(NodeType::TRIG, 0., NUM_PARAMS)
{
}

int Trig::lookupParam(std::string str) {
	return m_lookup[str];
}

float Trig::process()
{
	return params[TRIG];
}

