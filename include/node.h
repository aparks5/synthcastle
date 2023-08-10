#pragma once
#include "nodetypes.h"
#include <vector>

class Node
{
public:
	NodeType type;
	std::vector<float> params;
	int id;
	float value;

	Node(NodeType type, int id)
		: type(type)
		, id(id)
		, value(0.)
	{}

	Node(NodeType type, int id, float val)
		: type(type)
		, id(id)
		, value(val)
	{}

	virtual ~Node() {};
	virtual float process(float in) { return 0; }
	// some nodes are output only e.g. oscillators/lfos
	virtual float process() { return 0; }
	virtual void display() {};
	virtual void update() {}; // update should only
	// be called if a param changed in a node
	// we can tell if a node changed if the new value
	// of the UI is not equal to the nodes current value
	// otherwise we will end up performing an insane
	// amount of update functions every frame

private:
};