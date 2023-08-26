#include "trig.h"

Trig::Trig()
	: Node(NodeType::TRIG, 0., NUM_PARAMS)
	, m_sampsPerBeat(0)
	, m_counter(0)
	, m_cycle(0)
{
	memset(m_trigs, 0, sizeof(m_trigs));
}

int Trig::lookupParam(std::string str) {
	return m_lookup[str];
}

float Trig::process()
{
	if (m_cycle == 0) {
		if (params[BPM] != 0) {
			m_sampsPerBeat = 60 / params[BPM] * 44100;
			if (m_counter == 0) {
				m_counter++;
				if (m_counter >= m_sampsPerBeat) {
					m_counter = 0;
				}
				for (auto& t : m_trigs) {
					t = 1;
				}
			}
			else {
				m_counter++;
				if (m_counter >= m_sampsPerBeat) {
					m_counter = 0;
				}
				for (auto& t : m_trigs) {
					t = 0;
				}
			}
		}
	}

	m_cycle++;

	if (m_cycle >= 4) {
		m_cycle = 0;
	}
	return m_trigs[m_cycle];

}

