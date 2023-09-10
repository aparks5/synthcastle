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
	void display() override;

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
		EXTRA_OUT_ID,
		NUM_PORTS,
		NUM_VOICES,
        SELECTED_PORT,
	    NOTE,
		VELOCITY,
		NUM_PARAMS
	};
    MIDIUserData midiUserData;

private:
	std::shared_ptr<RtMidiIn> midiin;
	std::vector<std::string> ports;
	static void midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData);
	std::vector<float> m_voices;
	size_t m_currentVoice;
	size_t m_cycle;
	size_t m_numVoices;
	// to throttle updates that would resize the number of voices
	size_t m_cachedNumVoices;

};


#endif // MIDI_H_
