#pragma once

enum NodeType
{
	CONSTANT, // for a single unchanging value 
	DELAY,
	ENVELOPE,
	FILTER,
	FOUR_VOICE,
	GAIN,
	MIDI_IN,
	OSCILLATOR,
	OUTPUT,
	QUAD_MIXER,
	RELAY,  // used for routing multi output
	SAMPLER,
	SEQ, // 8 step sequencer
	TRIG, // button press outputs 1 then all 0s
	VALUE, // used to route inputs to node params
	NUM_NODE_TYPES
};