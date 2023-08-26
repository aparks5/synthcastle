#include "trig.h"

Trig::Trig()
	: Node(NodeType::TRIG, 0., NUM_PARAMS)
	, m_sampsPerBeat(0)
	, m_counter(0)
{
}

int Trig::lookupParam(std::string str) {
	return m_lookup[str];
}

float Trig::process()
{
	if (params[BPM] != 0) {
		m_sampsPerBeat = 60 / params[BPM] * 44100;
		if (m_counter == 0) {
			m_counter++;
			return 1;
		}
		else {
			m_counter++;
			if (m_counter >= m_sampsPerBeat) {
				m_counter = 0;
			}
			return 0;
		}
	}
	else {
		return params[TRIG];
	}
}

