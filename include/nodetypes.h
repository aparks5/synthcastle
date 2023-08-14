#pragma once

enum NodeType
{
	CONSTANT, // for a single unchanging value 
	DELAY,
	FILTER,
	GAIN,
	MIDI_IN,
	OSCILLATOR,
	OUTPUT,
	QUAD_MIXER,
	VALUE, // used to route inputs to node params
	NUM_NODE_TYPES
};