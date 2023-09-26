#include "sampler.h"

#include <fstream>
#include <ios>
#include <vector>
#include <cfloat>
#include "module.h"
#include "util.h"

Sampler::Sampler()
	: Node(NodeType::SAMPLER, "sampler", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
	, m_sampleRate(44100)
	, m_startPos(0)
	, m_accum(0)
	, m_increment(1.)
	, m_rate(1.)
	, m_path("")
	, m_bTriggered(false)
	, m_loopPoint(0)
{
	stringParams["path"] = "";
	m_env.params[Envelope::ATTACK_MS] = 100;
	m_env.params[Envelope::DECAY_MS] = 250;
	m_env.params[Envelope::SUSTAIN_DB] = -30.;
	m_env.params[Envelope::RELEASE_MS] = 50;

	paramMap = {
		{"node_id", NODE_ID},
		{"position_id", POSITION_ID},
		{"position", POSITION},
		{"spread", SPREAD},
		{"distance", DISTANCE},
		{"num_voices", NUM_VOICES},
		{"grainsize", GRAINSIZE},
		{"grainsize_mod", GRAINSIZE_MOD},
		{"spray", SPRAY},
		{"pitch_id", PITCH_ID},
		{"pitch", PITCH},
		{"filename", FILENAME},
		{"startstop_id", STARTSTOP_ID},
		{"startstop", STARTSTOP},
	};

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
		m_startPos = audioFile.getNumSamplesPerChannel();
		m_accum = audioFile.getNumSamplesPerChannel();
	}
}

float Sampler::process()
{
	if (params[STARTSTOP] >= 0.4) {
		params[STARTSTOP] = 0;
		m_env.params[Envelope::TRIG] = 1;
		m_startPos = 0;
		m_accum = 0;
		m_bTriggered = true;
	}

	if (params[POSITION] != 0 && m_bTriggered) {
		m_bTriggered = false;
		m_startPos = audioFile.getNumSamplesPerChannel() * params[SPREAD] * params[POSITION] ;
		m_accum = m_startPos;
		params[POSITION] = 0;
	}

	if (params[GRAINSIZE] != 0) {
		m_loopPoint = m_startPos + ((params[GRAINSIZE] / 1000.f) * m_sampleRate);
	}
	else {
		m_loopPoint = 0; // just return 0 instead of looping
	}

	if (params[PITCH] == 0) {
		return m_env.process(0);
	}

	if (params[PITCH] < 0.15) {
		return m_env.process(0);
	}
	else {
		m_rate = params[PITCH];
	}
	

	// playing back at 44.1kHz, advance only 1 sample each time
	m_accum += m_rate*m_increment;
	// linearInterpolate()

	if ((m_loopPoint != 0) && (m_accum > m_loopPoint)) {
		// start loop over immediately, 0 delay 
		m_accum = m_startPos;
	}

	if (m_accum > (audioFile.getNumSamplesPerChannel() - 1)) {
		return m_env.process(0.f);
	}
	else {
		int nearest = (int)m_accum;
		float remainder = fmodf(m_accum, nearest);

		if (nearest >= 1) {
			return m_env.process(linearInterpolate(audioFile.samples[0][nearest - 1], audioFile.samples[0][nearest], remainder));
		} 
		else {
			return m_env.process(audioFile.samples[0][0]);
		}
	}
}

