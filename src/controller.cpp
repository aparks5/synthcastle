#include "controller.h"

Controller::Controller(std::shared_ptr<Model> model)
	: m_pool(500)
	, m_model(model)
	, m_bUpdated(true)
	, m_bExit(false)
{
	// something should cause controller to exit eventually
}

ViewBag Controller::snapshot()
{
	// return a cached viewbag if the model hasn't been updated
	//if (m_bUpdated) {
	//	// repopulate the cache
	//	m_bUpdated = false;
	//}
	return m_model->refresh();
}

void Controller::notify(EventType event, const void* data) {
}

void Controller::queueCreation(std::string nodeType)
{
	m_creationQueue.push(nodeType);
}

void Controller::queueLink(int from, int to)
{
	LinkEvent link(from, to);
	m_linkQueue.push(link);
}

void Controller::queueUpdate(int id, std::string param, float val)
{
	UpdateEvent update(id, param, val);
	m_updates.push(update);
}

void Controller::queueUpdate(int id, std::string param, std::string str)
{
	UpdateStringEvent update(id, param, str);
	m_stringUpdates.push(update);
}

int Controller::createNode(std::string nodeType)
{
	return m_model->create(nodeType);
}

void Controller::createLink(int from, int to)
{
	m_model->link(from, to);
}

void Controller::update()
{
	// ideally this happens infrequently on its own separate thread, but that thread needs to keep track of the references
	m_pool.release();

	// do we need to update?
	if ((m_creationQueue.empty()) &&
		(m_updates.empty()) &&
		(m_stringUpdates.empty()) &&
		(m_linkQueue.empty())) {
		return;
	}

	
	while (!m_linkQueue.empty()) {
		auto link = m_linkQueue.front();
		m_linkQueue.pop();
		createLink(link.from, link.to);
	}

	while (!m_creationQueue.empty()) {
		auto create = m_creationQueue.front();
		m_creationQueue.pop();
		createNode(create);
	}

	while (!m_updates.empty()) {
		auto update = m_updates.front();
		// updating
		m_model->update(update);
		m_updates.pop();
	}

	while (!m_stringUpdates.empty()) {
		auto update = m_stringUpdates.front();
		// updating
		m_model->update(update);
		m_stringUpdates.pop();
	}

	// i think this means we need to copy the whole graph
	std::shared_ptr<NodeGraph> newGraph = std::make_shared<NodeGraph>(m_model->cloneGraph());
	m_pool.add(newGraph);
	std::atomic_store(&m_graph, newGraph);

}
