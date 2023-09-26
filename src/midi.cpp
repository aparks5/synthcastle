#include "midi.h"
#include <functional>
#include <iostream>
#include "imnodes.h"
#include "util.h"



MIDI::MIDI()
	: Node(NodeType::MIDI_IN, "midi_in", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
	, midiin(std::make_shared<RtMidiIn>())
	, m_portId(-1)
	, m_currentVoice(0)
	, m_numVoices(4)
	, m_cachedNumVoices(0)
	, lastActiveVoice(0)
	, prevFreq(0)
{
	m_voices.resize(4);
	midiin->setCallback(&MIDI::midiCallback, &midiUserData);

	// populate the MIDI port list at initialization
	unsigned int nPorts = midiin->getPortCount();
	if (nPorts == 0) {
		// todo: log
		std::cout << "No output ports available!" << std::endl;
	}
	else {
		for (unsigned int i = 0; i < nPorts; i++) {
			ports.push_back(midiin->getPortName(i));
		}
		params[NUM_PORTS] = nPorts;
	}

	paramMap = {
		{"node_id", NODE_ID},
		{"num_ports", NUM_PORTS},
		{"selected_port", SELECTED_PORT},
		{"out_voice1_id", OUT_VOICE1_ID},
		{"out_voice2_id", OUT_VOICE2_ID},
		{"out_voice3_id", OUT_VOICE3_ID},
		{"out_voice4_id", OUT_VOICE4_ID},
		{"out_voice1", OUT_VOICE1},
		{"out_voice2", OUT_VOICE2},
		{"out_voice3", OUT_VOICE3},
		{"out_voice4", OUT_VOICE4},
		{"note", NOTE},
		{"velocity", VELOCITY},
		{"out_velocity_id", OUT_VELOCITY_ID},
	};

}

void MIDI::midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData)
{
    MIDIUserData* pUserData = (MIDIUserData*)userData;
	// midi note to freq formula https://newt.phys.unsw.edu.au/jw/notes.html
	double stamp = 0;
	auto nBytes = 0;
	nBytes = message->size();
	if (nBytes == 3) {
		int byte0 = (int)message->at(0);
		auto note = (int)message->at(1);
		int velocity = (int)message->at(2);
		pUserData->velocity = velocity;
		if (byte0 == 144) {
			if (velocity != 0) {
				pUserData->note = note;
				//printf("note on: %d\n", pUserData->note);
			}
			else {
				pUserData->note = 0;
				//printf("note off: %d\n", pUserData->note);
			}
		}
		else if (byte0 == 128) {
			pUserData->note = 0;
			//printf("note off: %d\n", pUserData->note);
		}
	}
}



float MIDI::process()
{

	if (params[SELECTED_PORT] != m_portId) {
		m_portId = params[SELECTED_PORT];
		midiin->closePort();
		midiin->openPort((int)m_portId);
	}

	// all voices are exhausted, we need to turn one off

	if (midiUserData.note > 21) {
		params[NOTE] = midiUserData.note;
	}
	else {
		params[NOTE] = 0;
	}

	params[VELOCITY] = midiUserData.velocity;

	float freq = params[NOTE];
	// next problem, which voice changed to 0?
	// note off should be for one voice only,
	// but which voice?
	if (params[VELOCITY] == 0) {
		for (auto& v : m_voices) {
			v = 0;
		}
	}

	int activeVoices = 0;
	for (auto& v : m_voices) {
		if (v != 0) {
			activeVoices++;
		}
		else { // we found an unused voice
			if (freq != prevFreq) {
				v = freq;
				prevFreq = freq;
				lastActiveVoice++;
				if (lastActiveVoice > 4) {
					lastActiveVoice = 0;
				}
			}
			break;

		}

		// all voices are exhausted we need to turn one off
		if (activeVoices > 4) {
			m_voices[lastActiveVoice] = midiNoteToFreq((int)(freq));
			lastActiveVoice++;
			if (lastActiveVoice > 4) {
				lastActiveVoice = 0;
			}
		}
	}

	params[OUT_VOICE1] = m_voices[0];
	params[OUT_VOICE2] = m_voices[1];
	params[OUT_VOICE3] = m_voices[2];
	params[OUT_VOICE4] = m_voices[3];



	return 0;

}

