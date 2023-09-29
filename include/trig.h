#pragma once

#include "node.h"
#include <unordered_map>

// trigger outputs 1 on a button press then 0s until the next button press
class Trig : public Node
{
public:
	Trig();
	virtual ~Trig() {};
	void process() noexcept override;

	enum Inputs {
		NUM_INPUTS
	};

	enum Outputs {
		TRIGOUT,
		NUM_OUTPUTS
	};

	enum Params {
		NODE_ID,
		TRIGOUT_ID,
		TRIG,
		PROGRESS,
		PROGRESS_DIR,
		START,
		STOP,
		BPM,
		NUM_PARAMS
	};

private:
	std::unordered_map<std::string, int> m_lookup = {
			{"node_id", NODE_ID},
			{"trigout_id", TRIGOUT_ID},
			{"trigout", TRIGOUT},
			{"progress", PROGRESS},
			{"progress_dir", PROGRESS_DIR},
			{"trig", TRIG},
			{"bpm", BPM},
			{"start", START},
			{"stop", STOP}
	};
	size_t m_sampsPerBeat;
	size_t m_counter;
	float m_cache;

};
