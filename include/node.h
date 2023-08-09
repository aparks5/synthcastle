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
	virtual float process() { return 0; }
	virtual void display() {};

private:
};