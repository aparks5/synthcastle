#pragma once
#include "nodetypes.h"
#include <vector>
#include <string>
#include <unordered_map>

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
		, stringParams()
	{}

	Node(const Node& n) {
		type = n.type;
		value = n.value;
	}

	NodeType type;
	float value;
	std::vector<float> params;
	// for samplers, sometimes we need paths
	std::unordered_map<std::string, std::string> stringParams;

	virtual ~Node() {};
	virtual float process(float in) { return 0; }
	// some nodes are output only e.g. oscillators/lfos
	virtual float process() { return 0; }
	virtual void display() {};
	virtual void update() {};
	virtual int lookupParam(std::string str) { return -1; }
	std::string lookupString(std::string str) {
		if (stringParams.find(str) != stringParams.end()) {
			return stringParams[str];
		}

		return "";
	}
	virtual std::vector<std::string> paramStrings() { return {}; }

};