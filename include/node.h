#pragma once
#include "nodetypes.h"
#include <vector>
#include <string>

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
	std::vector<float> params;

	virtual ~Node() {};
	virtual float process(float in) { return 0; }
	// some nodes are output only e.g. oscillators/lfos
	virtual float process() { return 0; }
	virtual void display() {};
	virtual void update() {};
	virtual int lookupParam(std::string str) { return -1; }
	virtual std::vector<std::string> paramStrings() { return {}; }

};