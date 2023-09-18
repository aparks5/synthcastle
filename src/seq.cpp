#include "seq.h"

Seq::Seq()
	: Node(NodeType::SEQ, 0., NUM_PARAMS)
	, m_step(0)
	, m_peek(0)
	, m_lastEnabledVal(0)
{
	for (auto& en : m_enabled) {
		en = 1;
	}
}

int Seq::lookupParam(std::string str) {
	return m_lookup[str];
}

float Seq::process()
{

	if (params[RESET] == 1) {
		params[RESET] = 0;
		m_step = 0;
	}

	if (params[GATEMODE] && (params[TRIGIN] == 0)) {
		return 0;
	}

	params[STEP] = m_step;
	m_notes[0] = params[S1];
	m_notes[1] = params[S2];
	m_notes[2] = params[S3];
	m_notes[3] = params[S4];
	m_notes[4] = params[S5];
	m_notes[5] = params[S6];
	m_notes[6] = params[S7];
	m_notes[7] = params[S8];

	m_enabled[0] = params[S1_ON];
	m_enabled[1] = params[S2_ON];
	m_enabled[2] = params[S3_ON];
	m_enabled[3] = params[S4_ON];
	m_enabled[4] = params[S5_ON];
	m_enabled[5] = params[S6_ON];
	m_enabled[6] = params[S7_ON];
	m_enabled[7] = params[S8_ON];

	float val = 0;
	val = m_lastEnabledVal;

	if (params[TRIGIN] != 0) {
		params[TRIGIN] = 0;
		if (m_enabled[m_step] == 1) {
			val = m_notes[m_step];
			m_lastEnabledVal = val;
		}
		m_step++;
		if (m_step> 7) {
			m_step = 0;
		}
	}

	return val;

}

