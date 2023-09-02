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
	, m_bTriggered(false)
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
	}
}

float Sampler::process()
{
	if (params[STARTSTOP] >= 0.4) {
		params[STARTSTOP] = 0;
		m_pos = 0;
		m_accum = 0;
		m_bTriggered = true;
	}

	if (params[POSITION] != 0 && m_bTriggered) {
		m_bTriggered = false;
		m_pos = audioFile.getNumSamplesPerChannel() * params[POSITION];
		m_accum = m_pos;
		params[POSITION] = 0;
	}

	if (params[PITCH] == 0) {
		return 0;
	}

	m_rate = params[PITCH];
	

	// playing back at 44.1kHz, advance only 1 sample each time
	m_accum += m_rate*m_increment;
	// linearInterpolate()
	if (m_accum > (audioFile.getNumSamplesPerChannel() - 1)) {
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

