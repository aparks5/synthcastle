#include "note.h"
#include "util.h"

std::queue<NoteEvent> NoteGenerator::loopSequence(std::string input, size_t nTimes)
{
	std::queue<NoteEvent> noteEvents;
	for (size_t i = 0; i < nTimes; i++) {
		makeSequence(input);
	}
	return noteEvents;
}

std::queue<NoteEvent> NoteGenerator::makeSequence(std::string input)
{
	std::queue<NoteEvent> noteEvents;
	
	char noteGroup = '-';
	char eventGroup = ',';
	char durationSpecifier = ':';

	// get events first
	auto events = tokenize(input, ',');
	// now use `events`
	for (auto event : events) {
		// split notes into duration and note
		auto durationSplit = tokenize(event, ':');
		auto notes = tokenize(durationSplit[0], '-');

		std::string duration = "0";
		if (durationSplit.size() > 1) {
			duration = durationSplit[1];
		}
		auto numNotes = notes.size();
		for (auto note : notes) {
			numNotes--;
			auto dur = 0.f;
			if (numNotes == 0) {
				dur = stof(duration);
			}
			auto ev = makeNote(stod(note), true, dur, 1);
			noteEvents.push(ev);
		}
	}

	return noteEvents;
}

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
