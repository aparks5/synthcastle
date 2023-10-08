#pragma once

#include "nodegraph.h"
#include "viewbag.h"
#include "events.h"

#include <stack>

class ProcessorNodeCreator
{
public:
	ProcessorNodeCreator(std::shared_ptr<NodeGraph> g, ViewBag& v)
		: m_g(g)
		, m_v(v) {}
	~ProcessorNodeCreator() {};
	int create(std::string str);
	std::shared_ptr<NodeGraph> m_g;
	ViewBag& m_v;
	void cacheParam(int id, std::string param, float value);
	void cacheInput(int id, std::string in, float value);
	void cacheOutput(int id, std::string out, float value);
	void cacheString(int id, std::string param, std::string str);
	void cacheType(int id, NodeType t);
	void cacheName(int id, std::string name);
	std::pair<float, float> evaluate();
};

class Model
{
public:
	Model();
	virtual ~Model() {}
	int update(UpdateEvent update);
	int update(UpdateStringEvent update);
	int create(std::string str);
	void link(int from, int to);
	void deleteLink(int link_id);
	std::vector<std::string> queryNodeNames() const;
	std::tuple<float, float> evaluate();
	ViewBag refresh(); 
	// return a deep copy of the graph
	NodeGraph cloneGraph();
	std::stack<int> getTraversal() const { return m_postorder; }

private:
	std::shared_ptr<NodeGraph> m_graph;
	std::stack<int> m_postorder;
	ProcessorNodeCreator nodeCreator;
	ViewBag m_cache;

	std::vector<std::string> names = {
		"audio_input",
		"constant",
		"delay",
		"distort",
		"envelope",
		"filter",
		"freqshift",
		"gain",
		"looper",
		"mixer",
		"midi",
		"oscillator",
		"output",
		"sampler",
		"seq",
		"trig"
	};

};