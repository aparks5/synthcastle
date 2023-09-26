#pragma once
#include "nodetypes.h"
#include <vector>
#include <string>
#include <unordered_map>

class Node
{
public:

	Node(NodeType t, std::string name, int numInputs, int numOutputs, int numParams)
		: nodeType(t)
		, name(name)
		, stringParams()
	{
		inputs.resize(numInputs);
		outputs.resize(numOutputs);
		params.resize(numParams);
	}


	NodeType getNodeType() const { return nodeType; }

	std::vector<float> inputs;
	std::vector<float> outputs;
	std::vector<float> params;

	// for samplers, sometimes we need paths
	std::unordered_map<std::string, std::string> stringParams;

	virtual ~Node() {};
	virtual float process(float in) { return 0; }
	//virtual void process() {}; // < this is what we want for all nodes that adhere to the ins/params/outs pattern
	// some nodes are output only e.g. oscillators/lfos
	virtual float process() { return 0; }
	virtual void display() {};
	virtual void update() {};

	std::string paramStringByName(std::string str) {
		if (stringParams.find(str) != stringParams.end()) {
			return stringParams[str];
		}

		return "";
	}

	int paramIndexByName(std::string str)
	{
		return paramMap[str];
	}

	int inputIndexByName(std::string str)
	{
		return inputMap[str];
	}

	int outputIndexByName(std::string str)
	{
		return outputMap[str];
	}

	std::vector<std::string> paramStrings() 
	{
		std::vector<std::string> strings;
		for (std::unordered_map<std::string, int>::iterator iter = paramMap.begin(); iter != paramMap.end(); ++iter) {
			auto k = iter->first;
			strings.push_back(k);
		}

		return strings;
	}

protected:
	NodeType nodeType;
	std::string name;
	std::unordered_map<std::string, int> inputMap;
	std::unordered_map<std::string, int> paramMap;
	std::unordered_map<std::string, int> outputMap;

private:
	
};