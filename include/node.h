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

	virtual ~Node() {};

	NodeType getNodeType() const { return nodeType; }
	std::string getName() const { return name; }

	std::vector<float> inputs;
	std::vector<float> outputs;
	std::vector<float> params;
	std::unordered_map<std::string, std::string> stringParams;

	virtual void process() noexcept {};
	virtual void display() {};
	virtual void update() {};

	std::string paramStringByName(std::string str)
	{
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

	std::vector<std::string> inputStrings() 
	{
		std::vector<std::string> strings;
		for (std::unordered_map<std::string, int>::iterator iter = inputMap.begin(); iter != inputMap.end(); ++iter) {
			auto k = iter->first;
			strings.push_back(k);
		}

		return strings;
	}

	std::vector<std::string> outputStrings() 
	{
		std::vector<std::string> strings;
		for (std::unordered_map<std::string, int>::iterator iter = outputMap.begin(); iter != outputMap.end(); ++iter) {
			auto k = iter->first;
			strings.push_back(k);
		}

		return strings;
	}

	std::vector<std::string> inputIds() const
	{
		return inputIdStrings;
	}

	std::vector<std::string> outputIds() const
	{
		return outputIdStrings;
	}

	std::string inputIdToString(int id) const
	{
		if (id >= 0 && id < inputIdStrings.size()) {
			return inputIdStrings[id];
		}
		else {
			return "";
		}
	}

	std::string outputIdToString(int id) const
	{
		if (id >= 0 && id < outputIdStrings.size()) {
			return outputIdStrings[id];
		}
		else {
			return "";
		}
	}

protected:
	NodeType nodeType;
	std::string name;
	std::unordered_map<std::string, int> inputMap;
	std::unordered_map<std::string, int> paramMap;
	std::unordered_map<std::string, int> outputMap;
    std::vector<std::string> inputIdStrings;
    std::vector<std::string> outputIdStrings;
	void initIdStrings()
	{
		for (auto& [k, v] : inputMap) {
			inputIdStrings.push_back(k + "_id");
		}
		for (auto& [k, v] : outputMap) {
			outputIdStrings.push_back(k + "_id");
		}

	}
	
};