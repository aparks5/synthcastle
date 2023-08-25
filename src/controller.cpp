#include "controller.h"

Controller::Controller(std::shared_ptr<Model> model)
	: m_model(model)
	, m_bUpdated(true)
{
}

std::tuple<float, float> Controller::evaluate()
{
	return m_model->evaluate();
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
	return m_model->create(nodeType);
}

void Controller::createLink(int from, int to)
{
	m_model->link(from, to);
}

void Controller::update()
{
	std::scoped_lock lock{m_mut};

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
		// updating
		m_model->update(update);
		m_updates.pop();
	}
}
