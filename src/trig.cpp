#include "trig.h"


Trig::Trig()
	: Node(NodeType::PROCESSOR, "trig", NUM_INPUTS, NUM_PARAMS, NUM_OUTPUTS)
	, m_sampsPerBeat(0)
	, m_counter(0)
{
	params[PROGRESS_DIR] = 1.f;
}

void Trig::process() noexcept 
{
	float val = 0;

	if (params[STOP] == 1) {
		val = 0;
		m_counter = 0;
	}
	else if (params[TRIG] == 1) {
		params[TRIG] = 0;
		val = 1;
		m_counter = 0;
	}
	else if (params[BPM] != 0) {
		m_sampsPerBeat = 60 / params[BPM] * 44100;
		m_counter++;
		if (m_counter >= m_sampsPerBeat) {
			val = 1;
			m_counter = 0;
		}
	}

	outputs[TRIGOUT] = val;

}

