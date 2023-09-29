#include "looper.h"

#include <fstream>
#include <ios>
#include <vector>
#include <cfloat>
#include "util.h"

Looper::Looper()
	: Node(NodeType::PROCESSOR, "looper", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
	, m_sampleRate(44100)
	, m_numLayers(0)
	, m_maxLoopTimeSec(60)
	, m_startPos(0)
	, m_accum(0)
	, m_increment(1.)
	, m_rate(1.)
	, m_path("")
	, m_bTriggered(false)
	, m_loopPointBegin(0)
	, m_loopPointEnd(0)
	, m_bRecording(false)
	, m_bStop(false)
	, m_bErase(false)
	, m_bReverse(false)

{
	auto maxNumSamps = m_sampleRate * m_maxLoopTimeSec;
	m_loop.reserve(maxNumSamps);
	m_buffer.reserve(maxNumSamps);
	stringParams["path"] = "";

	paramMap = {
		{"node_id", NODE_ID},
		{"stop", STOP},
		{"record", RECORD},
		{"erase", ERASE},
		{"loop", LOOP},
		{"loop_point_begin", LOOP_POINT_BEGIN},
		{"loop_point_end", LOOP_POINT_END},
		{"reverse", REVERSE},
		{"speed", SPEED},
		{"filename", FILENAME},
	};

	inputMap = {
		{"input", INPUT}
	};

	outputMap = {
		{"output", OUTPUT}
	};

	initIdStrings();

} 

void Looper::process() noexcept
{

	float in = inputs[INPUT];

	if (m_bRecording) {
		m_buffer.push_back(in);
		if ((m_numLayers > 1) && (m_buffer.size() >= m_maxBufferSize)) {
			m_buffer.clear();
		}
	}

	// the first layer governs the buffer size
	if (params[STOP] == 1) {
		params[STOP] = 0;
		m_bRecording = false;
		m_accum = 0;
		m_numLayers++;
		if (m_numLayers == 1) {
			m_maxBufferSize = m_buffer.size(); // no more resizing after the first loop
			m_loop.resize(m_buffer.size());
		}
		if (m_numLayers > 0) {
			size_t loopSamples = 0;
			loopSamples = (m_loop.size() > m_buffer.size()) ? m_loop.size() : m_buffer.size();
			for (size_t idx = 0; idx < loopSamples; idx++) {
				// if idx > m_buffer.size(), insert a zero
				if (idx > (m_buffer.size() - 1)) {
					break;
				}
				m_loop[idx] = (0.707f * (m_loop[idx] + m_buffer[idx])); // should allow for different mix values
			}

		}
		m_loopPointBegin = 0;
		m_loopPointEnd = m_loop.size() - 1;
		m_buffer.clear();

	}

	if (params[LOOP] == 1) {
		params[STOP] = 0;
		params[LOOP] = 0;
		m_bRecording = true;
	}

	if (params[ERASE] == 1) {
		params[ERASE] = 0;
		m_numLayers = 0;
		m_maxBufferSize = 0;
		std::fill(m_loop.begin(), m_loop.end(), 0);
		std::fill(m_buffer.begin(), m_buffer.end(), 0);
		//out = 0.f;
	}

	// loop point begin / end should be 0 to 1 scaled to duration


	// playing back at 44.1kHz, advance only 1 sample each time
	m_accum += m_rate*m_increment;
	// linearInterpolate()

	if ((m_loopPointEnd != 0) && (m_accum > m_loopPointEnd)) {
		// start loop over immediately, 0 delay 
		m_accum = m_loopPointBegin;
	}

	int nearest = (int)m_accum;
	float remainder = fmodf(m_accum, nearest);

	float out = 0.f;

	if ((nearest >= 1) && (nearest <= m_loop.size())) {
		// insert drywet here
		out = (0.707*in) + (0.707*linearInterpolate(m_loop[nearest - 1], m_loop[nearest], remainder));
	} 
	else if (m_loop.size() == 0) {
		out = 0;
	}

	outputs[OUTPUT] = out;
}

