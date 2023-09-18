
#include "value.h"

Value::Value(float val)
	: Node(NodeType::VALUE, val, 0)
{
};

Value::Value()
	: Node(NodeType::VALUE, 0.f, 0)
{
};



