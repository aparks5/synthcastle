#include "sampler.h"

#include <fstream>
#include <ios>
#include <vector>
#include <cfloat>
#include "module.h"
#include "util.h"

Sampler::Sampler()
	: Node(NodeType::SAMPLER, 0.f, NUM_PARAMS)
	, m_sampleRate(44100)
	, m_pos(0)
	, m_accum(0)
	, m_increment(1.)
	, m_rate(1.)
	, m_path("")
	, m_bDone(false)
{
	stringParams["path"] = "";
}

// all node update functions should be called outside of the audio thread
// and all the relevant data and nodes should
// be copied before the graph is evaluated
void Sampler::update()
{
	if (stringParams["path"] != m_path) { // find a way to do
		// file io for samplers on a separate thread and out of the audio callback
		m_path = stringParams["path"];
		audioFile.load(stringParams["path"]);
		m_pos = audioFile.getNumSamplesPerChannel();
		m_accum = audioFile.getNumSamplesPerChannel();
		m_bDone = true;
	}
}

float Sampler::process()
{
	if (params[STARTSTOP] >= 0.4 && m_bDone) {
		params[STARTSTOP] = 0;
		m_bDone = false;
		m_pos = 0;
		m_accum = 0;
	}
	m_rate = 1; // midiNoteToFreq(params[INPUT]) / 440.;

	// playing back at 44.1kHz, advance only 1 sample each time
	m_accum += m_rate*m_increment;
	// linearInterpolate()
	if (m_accum > (audioFile.getNumSamplesPerChannel() - 1)) {
		m_bDone = true;
		return 0.f;
	}
	else {
		int nearest = (int)m_accum;
		float remainder = fmodf(m_accum, nearest);

		if (nearest >= 1) {
			return linearInterpolate(audioFile.samples[0][nearest - 1], audioFile.samples[0][nearest], remainder);
		} 
		else {
			return audioFile.samples[0][0];
		}
	}
}

