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


	std::shared_ptr<RtMidiIn> midiin;
	static void midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData);
    MIDIUserData midiUserData;
	bool chooseMidiPort(std::shared_ptr<RtMidiOut>rtmidi);
	bool chooseMidiInPort(std::shared_ptr<RtMidiIn>rtmidi);
};


#endif // MIDI_H_
