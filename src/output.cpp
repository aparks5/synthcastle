#include "output.h"
#include "imnodes.h"

Output::Output()
    : Node(NodeType::OUTPUT, 0., NUM_PARAMS)
{
}

float Output::process()
{
    return value;
}

	//m_rootNodeId = outputId;
	//m_graph.setRoot(m_rootNodeId);


