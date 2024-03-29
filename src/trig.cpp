#include "trig.h"


Trig::Trig()
	: Node(NodeType::PROCESSOR, "trig", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
	, m_sampsPerBeat(0)
	, m_counter(0)
	, m_bpm(0)
{
	paramMap = {
		{"node_id", NODE_ID},
		{"progress", PROGRESS},
		{"progress_dir", PROGRESS_DIR},
		{"trig", TRIG},
		{"bpm", BPM},
		{"start", START},
		{"stop", STOP}
	};

	outputMap = {
		{"trigout", TRIGOUT},
		{"trig8", TRIG8},
		{"trig16", TRIG16}
	};

	initIdStrings();

	params[PROGRESS_DIR] = 1.f;

}

void Trig::process() noexcept 
{

	outputs[TRIGOUT] = 0;
	outputs[TRIG8] = 0;
	outputs[TRIG16] = 0;

	if (params[TRIG] == 1) {
		outputs[TRIGOUT] = 1;
		params[TRIG] = 0;
		m_counter = 0;
		return;
	}
	else if (params[STOP] == 1) {
		outputs[TRIGOUT] = 0;
		m_counter = 0;
		return;
	}
	else if (params[BPM] != 0) {
		if (params[BPM] != m_bpm) {
			m_bpm = params[BPM];
			m_sampsPerBeat = roundf(60. / m_bpm * 44100);
			m_sampsPer8th = roundf(m_sampsPerBeat / 2);
			m_sampsPer16th = roundf(m_sampsPer8th / 2);
		}
		m_counter++;
		m_counter8++;
		m_counter16++;

		if (m_counter8 >= m_sampsPer8th) {
			outputs[TRIG8] = 1;
			m_counter8 = 0;
		}

		if (m_counter16 >= m_sampsPer16th) {
			outputs[TRIG16] = 1;
			m_counter16 = 0;
		}

		if (m_counter >= m_sampsPerBeat) {
			outputs[TRIGOUT]  = 1;
			m_counter = 0;
		}
	}

}

