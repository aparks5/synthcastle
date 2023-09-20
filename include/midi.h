#ifndef MIDI_H_
#define MIDI_H_

#include "rtmidi/RtMidi.h"
#include "node.h"
#include <string>
#include <queue>

#include "note.h"

class MIDI : public Node
{
public:
	MIDI();
	virtual ~MIDI() {}
	float process() override;

	int lookupParam(std::string str) override {
		return m_lookup[str];
	}
	std::vector<std::string> paramStrings() override
	{
		std::vector<std::string> strings;
		for (std::unordered_map<std::string, int>::iterator iter = m_lookup.begin(); iter != m_lookup.end(); ++iter) {
			auto k = iter->first;
			strings.push_back(k);
		}

		return strings;
	}


	struct MIDIUserData
	{
		int note;
		int velocity;
		MIDIUserData()
			: note(0)
			, velocity(0)
		{}
	};

	enum MidiParams
	{
		NODE_ID,
		NUM_PORTS,
		SELECTED_PORT,
		OUT_VOICE1_ID,
		OUT_VOICE2_ID,
		OUT_VOICE3_ID,
		OUT_VOICE4_ID,
		OUT_VOICE1,
		OUT_VOICE2,
		OUT_VOICE3,
		OUT_VOICE4,
	    NOTE,
		VELOCITY,
		OUT_VELOCITY_ID,
		NUM_PARAMS
	};

    MIDIUserData midiUserData;

private:
	std::shared_ptr<RtMidiIn> midiin;
	std::vector<std::string> ports;
	float m_portId;
	static void midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData);
	std::vector<float> m_voices;
	size_t m_currentVoice;
	size_t m_numVoices;
	// to throttle updates that would resize the number of voices
	size_t m_cachedNumVoices;

	int lastActiveVoice;
	float prevFreq;

	std::unordered_map<std::string, int> m_lookup =
	{
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

};


#endif // MIDI_H_
