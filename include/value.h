#pragma once

#include "node.h"

class Value : public Node
{
public:
	Value(float val);
	virtual ~Value() {};

};