#pragma once

#include "node.h"
#include <unordered_map>

// in evaluate() the PortAudio callback will see this node in the graph and simply push one
// value at a time to the top of the value stack
class AudioInput : public Node
{
public:
	AudioInput();
	virtual ~AudioInput() {};
	//int lookupParam(std::string str) override;

	enum AudioInputParams {
		NODE_ID,
		NUM_PARAMS
	};

private:
	std::unordered_map<std::string, int> m_lookup = {
			{"node_id", NODE_ID},
	};

};
