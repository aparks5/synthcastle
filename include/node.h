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

	Node(NodeType type, float val)
		: type(type)
		, value(val)
	{}

	Node(const Node& n) {
		type = n.type;
		value = n.value;
		params = n.params;
	}

	NodeType type;
	std::vector<float> params;
	float value;

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