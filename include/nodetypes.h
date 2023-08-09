#pragma once

enum NodeType
{
	VALUE, // used to route inputs to node params
	OSCILLATOR,
	GAIN,
	FILTER,
	DELAY,
	OUTPUT,
	NUM_NODE_TYPES
};