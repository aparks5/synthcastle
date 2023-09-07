#pragma once

#include "model.h"
#include "viewlistener.h"
#include "events.h"
#include "releasepool.h"
#include "audioplotbuffer.h"

#include <memory>
#include <queue>

class Controller : public ViewListener
{
public:
	Controller(std::shared_ptr<Model> model);
	virtual ~Controller() {}
	void notify(EventType event, const void* data) override;
	std::tuple<float, float> evaluate();
	void queueCreation(std::string nodeType) override;
	void queueLink(int from, int to) override;
	void queueUpdate(int id, std::string param, float val) override;
	void queueUpdate(int id, std::string param, std::string str) override;
	std::shared_ptr<NodeGraph> m_graph;

	void createLink(int from, int to);
	int createNode(std::string nodeTypeString);
	ViewBag snapshot() const override; 
	void update() override; // not accessible to View 
	// because this is not part of the ViewListener interface
	bool shouldExit() const {
		return m_bExit;
	}

	void sendBuffer(std::vector<std::vector<float>> buf);
	AudioPlotBuffer buffer() const override { return m_circBuff; }

private:
	ReleasePool m_pool;
	AudioPlotBuffer m_circBuff;
	std::mutex m_mut;
	std::shared_ptr<Model> m_model;
	ViewBag m_cache;
	bool m_bUpdated;
	std::queue<UpdateEvent> m_updates;
	std::queue<UpdateStringEvent> m_stringUpdates;
	std::queue<std::string> m_creationQueue;
	std::queue<LinkEvent> m_linkQueue;
	bool m_bExit;

};
