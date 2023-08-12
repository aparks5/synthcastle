#pragma once

enum NodeType
{
	VALUE, // used to route inputs to node params
	CONSTANT, // for a single unchanging value 
	OSCILLATOR,
	GAIN,
	FILTER,
	DELAY,
	OUTPUT,
	NUM_NODE_TYPES
};