#include "constant.h"

Constant::Constant()
	: Node(NodeType::CONSTANT, 0., NUM_PARAMS)
{
}

float Constant::process()
{
	// play with invalid val flag
	if (params[INPUT1] != 0) {
		params[VALUE1] = params[INPUT1];
	}

	if (params[INPUT2] != 0) {
		params[VALUE2] = params[INPUT2];
	}

	if (params[INPUT3] != 0) {
		params[VALUE3] = params[INPUT3];
	}

	if (params[INPUT4] != 0) {
		params[VALUE4] = params[INPUT4];
	}

	return 0;
}

