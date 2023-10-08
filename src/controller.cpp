#include "controller.h"

Controller::Controller(std::shared_ptr<Model> model)
	: m_pool(500)
	, m_model(model)
	, m_bUpdated(true)
	, m_bExit(false)
{
	// something should cause controller to exit eventually
	std::vector<float> myRow(44100, 0.f);
	for (size_t idx = 0; idx < 2; idx++) {
		m_circBuff.buffer.push_back(myRow);
	}
	m_circBuff.m_writeIdx = 0;
}

ViewBag Controller::snapshot() 
{
	// return a cached viewbag if the model hasn't been updated
	if (m_bUpdated) {
		auto bag = m_model->refresh();
		m_cache = bag;
	}

	return m_cache;
}

std::stack<int> Controller::getTraversal() const
{
	return m_model->getTraversal();
}

void Controller::notify(EventType event, const void* data) {
}

std::vector<std::string> Controller::queryNodeNames() const
{
	return m_model->queryNodeNames();
}

void Controller::queueCreation(std::string nodeName)
{
	m_creationQueue.push(nodeName);
}

void Controller::queueDestroyLink(int link_id)
{
	m_linkDeletionQueue.push(link_id);
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

void Controller::sendBuffer(std::vector<std::vector<float>> buf)
{
	for (size_t ch = 0; ch < 2; ch++) {
		size_t len = m_circBuff.buffer[ch].size();
		for (size_t idx = 0; idx < buf[ch].size(); idx++) {
			m_circBuff.buffer[ch][m_circBuff.m_writeIdx] = buf[ch][idx];
			m_circBuff.m_writeIdx++;
			if (m_circBuff.m_writeIdx >= len) {
				m_circBuff.m_writeIdx = 0;
			}
		}
	}
}

void Controller::queueUpdate(int id, std::string param, std::string str)
{
	UpdateStringEvent update(id, param, str);
	m_stringUpdates.push(update);
}

int Controller::createNode(std::string nodeName)
{
	return m_model->create(nodeName);
}

void Controller::createLink(int from, int to)
{
	m_model->link(from, to);
}

void Controller::deleteLink(int link_id)
{
	m_model->deleteLink(link_id);
}

void Controller::update()
{
	// ideally this happens infrequently on its own separate thread, but that thread needs to keep track of the references
	m_pool.release();

	m_bUpdated = false;

	// do we need to update?
	if ((m_creationQueue.empty()) &&
		(m_updates.empty()) &&
		(m_stringUpdates.empty()) &&
		(m_linkQueue.empty()) &&
		(m_linkDeletionQueue.empty())) {
		return;
	}

	while (!m_linkDeletionQueue.empty()) {
		auto linkIdToDelete = m_linkDeletionQueue.front();
		m_linkDeletionQueue.pop();
		deleteLink(linkIdToDelete);
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

	m_bUpdated = true;
	// i think this means we need to copy the whole graph
	std::shared_ptr<NodeGraph> newGraph = std::make_shared<NodeGraph>(m_model->cloneGraph());
	m_pool.add(newGraph);
	std::atomic_store(&m_graph, newGraph);

}
