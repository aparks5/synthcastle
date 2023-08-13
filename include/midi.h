#ifndef MIDI_H_
#define MIDI_H_

#include "RtMidi.h"
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
		NUM_PORTS,
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
};


#endif // MIDI_H_
