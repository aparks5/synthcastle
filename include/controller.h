#pragma once

#include "model.h"
#include "view.h"
#include "viewlistener.h"
#include "events.h"

#include <memory>
#include <queue>

class Controller : public ViewListener
{
public:
	Controller(std::shared_ptr<View> view, std::shared_ptr<Model> model);
	virtual ~Controller() {}
	void notify(EventType event, const void* data) override;
	std::tuple<float, float> evaluate();
	void queueCreation(std::string nodeType) override;
	void queueLink(int from, int to) override;
	void queueUpdate(int id, std::string param, float val) override;

	void createLink(int from, int to);
	int createNode(std::string nodeTypeString);
	ViewBag snapshot() override; 
	void update() override; // not accessible to View 
	// because this is not part of the ViewListener interface

private:
	std::shared_ptr<View> m_view;
	std::shared_ptr<Model> m_model;
	ViewBag m_cache;
	bool m_bUpdated;
	std::queue<UpdateEvent> m_updates;
	std::queue<std::string> m_creationQueue;
	std::queue<LinkEvent> m_linkQueue;

	std::unordered_map<std::string, NodeType> m_nodeTypeMap =
	{
		{"gain", NodeType::GAIN},
		{"oscillator", NodeType::OSCILLATOR},
		{"constant", NodeType::CONSTANT},
		{"output", NodeType::OUTPUT}
	};
};
