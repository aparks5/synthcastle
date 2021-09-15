#ifndef MIDI_H_
#define MIDI_H_

#include "RtMidi.h"

#include "mixerstream.h"
#include "note.h"

#include <queue>

class MIDI
{
public:
	MIDI(MixerStream& stream);

private:
	struct MIDIUserData
	{
		MixerStream* stream;
		std::queue<NoteEvent> notes;
		MIDIUserData() : stream(nullptr) {};
	};

	static void midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData);
    MIDIUserData midiUserData;
	bool chooseMidiPort(RtMidiOut* rtmidi);
	bool chooseMidiInPort(RtMidiIn* rtmidi);
};


#endif // MIDI_H_
