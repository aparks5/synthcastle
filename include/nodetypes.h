#pragma once

enum NodeType
{
	CONSTANT, // for a single unchanging value 
	DELAY,
	FILTER,
	FOUR_VOICE,
	GAIN,
	MIDI_IN,
	OSCILLATOR,
	OUTPUT,
	QUAD_MIXER,
	RELAY,  // used for routing multi output
	VALUE, // used to route inputs to node params
	NUM_NODE_TYPES
};