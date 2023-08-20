#pragma once

#include "events.h"
#include "viewbag.h"

class ViewListener
{
public:

	virtual void notify(EventType type, const void* data) = 0;
	// viewbag should contain only the data necessary for presentation
	// in our case thats the IDs and param values
	// not the node memory or processes()
	virtual int createNode(std::string nodeTypeString) = 0;
	virtual void queueCreation(std::string nodeType) = 0;
	virtual void queueUpdate(int id, std::string param, float val) = 0;
	virtual void queueLink(int from, int to) = 0;
	virtual ViewBag snapshot() = 0;
	virtual void update() = 0; // for now, should take out of interface eventually
};