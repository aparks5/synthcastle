#include "seq.h"

Seq::Seq()
	: Node(NodeType::SEQ, 0., NUM_PARAMS)
	, m_step(0)
{
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

	params[STEP] = m_step;
	m_notes[0] = params[S1];
	m_notes[1] = params[S2];
	m_notes[2] = params[S3];
	m_notes[3] = params[S4];
	m_notes[4] = params[S5];
	m_notes[5] = params[S6];
	m_notes[6] = params[S7];
	m_notes[7] = params[S8];

	float val = m_notes[m_step];
	if (params[TRIGIN] != 0) {
		params[TRIGIN] = 0;
		m_step++;
		if (m_step > 7) {
			m_step = 0;
		}
	}

	return val;

}

