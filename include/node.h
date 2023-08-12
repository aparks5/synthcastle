#pragma once
#include "nodetypes.h"
#include <vector>

class Node
{
public:

	Node(NodeType type)
		: type(type)
		, value(0.)
	{}

	Node(NodeType type, float val, int numParams)
		: type(type)
		, value(val)
		, params(numParams)
	{}

	Node(const Node& n) {
		type = n.type;
		value = n.value;
	}

	NodeType type;
	float value;
	// subclasses should initialize this 
	// based on NUM_PARAMS at end of params enum
	// per class
	std::vector<float> params;

	virtual ~Node() {};
	virtual float process(float in) { return 0; }
	// some nodes are output only e.g. oscillators/lfos
	virtual float process() { return 0; }
	virtual void display() {};
	virtual void update() {}; // update should only
	virtual void link(const float* src, const float* dest) {};
	// be called if a param changed in a node
	// we can tell if a node changed if the new value
	// of the UI is not equal to the nodes current value
	// otherwise we will end up performing an insane
	// amount of update functions every frame

};