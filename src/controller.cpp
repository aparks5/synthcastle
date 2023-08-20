#include "controller.h"

Controller::Controller(std::shared_ptr<View> view, std::shared_ptr<Model> model)
	: m_view(view)
	, m_model(model)
	, m_bUpdated(true)
{

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

int Controller::createNode(std::string nodeType)
{
	// queue creation and linking
	// todo, use map to avoid long if statements
	if (nodeType == "gain") {
		return m_model->create(NodeType::GAIN);
	}
}

void Controller::createLink(int from, int to)
{
	m_model->link(from, to);
}

void Controller::update()
{
	m_bUpdated = ((!m_creationQueue.empty()) &&
				  (!m_updates.empty()) &&
				  (!m_linkQueue.empty()));
	
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
		m_model->update(update);
		m_updates.pop();
	}
}
