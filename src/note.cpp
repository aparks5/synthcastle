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
	char trackSpecifier = '=';
	char eventGroup = ',';
	char durationSpecifier = ':';

	// get events first
	auto trackSplit = tokenize(input, '=');
	auto trackNum = 1;

	auto eventIdx = 0;

	if (trackSplit.size() > 1) {
		trackNum = std::stod(trackSplit[0]);
		eventIdx = 1;
	}
	else {
		eventIdx = 0;
	}

	auto events = tokenize(trackSplit[eventIdx], ',');

	// now use `events`
	auto timestamp = 0.;
	
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
			if (numNotes == notes.size()) {
				timestamp += stof(duration);
			}
			timestamp = clamp(timestamp, 0., 8.);
			numNotes--;
			auto ev = makeNote(stod(note), true, timestamp, trackNum);
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
