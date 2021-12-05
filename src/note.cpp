#include "note.h"
#include "util.h"
#include "scale.h"

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
	std::string track = "";

	auto eventIdx = 0;

	if (trackSplit.size() > 1) {
		track = trackSplit[0];
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
			auto ev = makeNote(stod(note), true, timestamp, track);
			noteEvents.push(ev);
		}
	}

	return noteEvents;
}

std::queue<NoteEvent> NoteGenerator::randomPattern(std::string track, size_t numSteps, size_t lowNote, size_t highNote) 
{
	std::queue<NoteEvent> noteEvents;
	Scale scale(Key::A, ScalePattern::MINOR, ScaleMode::IONIAN);
	
	// now use `events`
	auto timestamp = 0.;
	float minDur = 0.0625;
	float maxDur = 0.250;

	for (size_t idx = 0; idx < numSteps; idx++) {
		// split notes into duration and note
		float randDur16ths = rand() % (4 - 1 + 1);
		timestamp += (randDur16ths*minDur);
		size_t randScaleIdx = rand() % (scale.length() + 1);
		auto ev = makeNote(scale(randScaleIdx), true, timestamp, track);
		noteEvents.push(ev);
	}

	return noteEvents;
}

std::queue<NoteEvent> NoteGenerator::scalePattern(Key key, ScalePattern pattern, ScaleMode mode)
{
	(void)key;
	(void)pattern;
	(void)mode;
	std::queue<NoteEvent> noteEvents;
	Scale scale(key, pattern, mode);
	
	// now use `events`
	auto timestamp = 0.;
	float minDur = 0.0625; // 1/16 note
	float maxDur = 0.25; // 1/4 note
	std::string track = "synth1";

	for (size_t idx = 0; idx <= scale.length(); idx++) {
		// split notes into duration and note
		timestamp += minDur;
		size_t randScaleIdx = rand() % (scale.length() + 1);
		auto ev = makeNote(scale(idx), true, timestamp, track);
		noteEvents.push(ev);
	}

	return noteEvents;
}




NoteEvent NoteGenerator::makeNote(int noteVal, bool bNoteOn, float timeVal, std::string track)
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
