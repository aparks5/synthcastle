#include "trig.h"


Trig::Trig()
	: Node(NodeType::TRIG, 0., NUM_PARAMS)
	, m_sampsPerBeat(0)
	, m_counter(0)
	, m_cycle(0)
	, m_cache(0)
{
	memset(m_trigs, 0, sizeof(m_trigs));
	params[NUMTRIGS] = NUM_TRIGS;
}

int Trig::lookupParam(std::string str) {
	return m_lookup[str];
}

float Trig::process()
{
	if (params[TRIG] == 1) {
		return 1;
	}

	if (params[STOP] == 1) {
		return 0;
		m_cycle = 0;
	}
	
	if (m_cycle != 0) {
		// if we're cycling thru outputs dont update m_counter
		m_cycle++;
		if (m_cycle > params[NUMTRIGS]) {
			m_cycle = 0;
		}
		return m_cache;
	}

	if (params[BPM] != 0) {
		m_sampsPerBeat = 60 / params[BPM] * 44100;
		if (m_counter == 0) {
			m_counter++;
			if (m_counter >= m_sampsPerBeat) {
				m_counter = 0;
			}
			m_cache = 1;
			m_cycle++;
		}
		else {
			m_counter++;
			if (m_counter >= m_sampsPerBeat) {
				m_counter = 0;
			}
			m_cache = 0;
		}
	}

	return m_cache;
}

