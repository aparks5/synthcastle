#include "note.h"

NoteEvent NoteGenerator::makeNote(int noteVal, bool bNoteOn, float timeVal, int track)
{
	NoteEvent note;
	std::vector<unsigned char> message;
	message.push_back(0);
	message.push_back(0);
	message.push_back(0);

	auto midiMsg = bNoteOn ? 144 : 128;

	// Send out a series of MIDI messages.
	message[0] = midiMsg;
	message[1] = noteVal;
	message[2] = 127;

	note.message = message;
	note.timeVal = timeVal;
	note.track = track;

	return note;


}
