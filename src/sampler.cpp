#include "sampler.h"

#include <fstream>
#include <ios>
#include <vector>
#include <cfloat>
#include "module.h"
#include "util.h"

Sampler::Sampler()
	: Node(NodeType::PROCESSOR, "sampler", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
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

	paramMap = {
		{"node_id", NODE_ID},
		{"spread", SPREAD},
		{"distance", DISTANCE},
		{"num_voices", NUM_VOICES},
		{"grainsize", GRAINSIZE},
		{"grainsize_mod", GRAINSIZE_MOD},
		{"spray", SPRAY},
		{"filename", FILENAME},
	};

	inputMap = {
		{"pitch", PITCH},
		{"position", POSITION},
		{"startstop", STARTSTOP}
	};

	outputMap = {
		{"output", OUTPUT}
	};

	initIdStrings();

	m_env.params[Envelope::ATTACK_MS] = 100;
	m_env.params[Envelope::DECAY_MS] = 250;
	m_env.params[Envelope::SUSTAIN_DB] = -30.;
	m_env.params[Envelope::RELEASE_MS] = 50;
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

void Sampler::process() noexcept
{
	update();
	if (inputs[STARTSTOP] >= 0.4) {
		inputs[STARTSTOP] = 0;
		m_env.inputs[Envelope::TRIG] = 1;
		m_startPos = 0;
		m_accum = 0;
		m_bTriggered = true;
	}

	if (inputs[POSITION] != 0 && m_bTriggered) {
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

	// TODO: the rest of this function is a mess starting here
	if (inputs[PITCH] == 0) {
		m_env.inputs[Envelope::INPUT] = 0;
		m_env.process();
		outputs[OUTPUT] = m_env.outputs[Envelope::OUTPUT];
		return;
	}

	if (inputs[PITCH] < 0.15) {
		m_env.inputs[Envelope::INPUT] = 0;
		m_env.process();
		outputs[OUTPUT] = m_env.outputs[Envelope::OUTPUT];
		return;
	}
	else {
		m_rate = inputs[PITCH];
	}
	

	// playing back at 44.1kHz, advance only 1 sample each time
	m_accum += m_rate*m_increment;
	// linearInterpolate()

	if ((m_loopPoint != 0) && (m_accum > m_loopPoint)) {
		// start loop over immediately, 0 delay 
		m_accum = m_startPos;
	}

	if (m_accum > (audioFile.getNumSamplesPerChannel() - 1)) {
		m_env.inputs[Envelope::INPUT] = 0;
		m_env.process();
		outputs[OUTPUT] = m_env.outputs[Envelope::OUTPUT];
		return;
	}
	else {
		int nearest = (int)m_accum;
		float remainder = fmodf(m_accum, nearest);

		if (nearest >= 1) {
			m_env.inputs[Envelope::INPUT] = linearInterpolate(audioFile.samples[0][nearest - 1], audioFile.samples[0][nearest], remainder);
			m_env.process();
			outputs[OUTPUT] = m_env.outputs[Envelope::OUTPUT];
			return;
		
		} 
		else {
			m_env.inputs[Envelope::INPUT] = audioFile.samples[0][0];
			m_env.process();
			outputs[OUTPUT] = m_env.outputs[Envelope::OUTPUT];
			return;
		}
	}
}

