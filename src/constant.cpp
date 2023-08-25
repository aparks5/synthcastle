#include "constant.h"

Constant::Constant()
	: Node(NodeType::CONSTANT, 0., NUM_PARAMS)
{
}

int Constant::lookupParam(std::string str) {
	return m_lookup[str];
}

float Constant::process()
{
	return params[VALUE];
}

