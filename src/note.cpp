#include "note.h"
#include "util.h"
#include "scale.h"

std::deque<NoteEvent> NoteGenerator::loopSequence(std::string input, size_t nTimes, std::vector<std::string> trackList)
{
	std::deque<NoteEvent> noteEvents;
	for (size_t i = 0; i < nTimes; i++) {
		makeSequence(input, trackList);
	}
	return noteEvents;
}

std::deque<NoteEvent> NoteGenerator::makeDrumPattern(float quantization, std::vector<std::string> input)
{
	std::deque<NoteEvent> noteEvents;

	char kick = 'x';
	char hat = 't';
	char rest = '-';
	char snare = 'o';
	char clap = 'c';

	// each char is an eigth note
	auto timestamp = 0.;
	for (auto& ch : input[0]) {

		// split notes into duration and note
		// 69 = 440Hz = A4
		std::string track;
		if (ch == kick) {
			track = "kick";
		}
		else if (ch == hat) {
			track = "hat";
		}
		else if (ch == snare) {
			track = "snare";
		}
		else if (ch == clap) {
			track = "clap";
		}

		if (ch != rest) {
			auto ev = makeNote(69, true, timestamp, track);
			noteEvents.push_back(ev);
		}

		timestamp += quantization;
	}

	return noteEvents;
}

std::deque<NoteEvent> NoteGenerator::makeSequence(std::string input, std::vector<std::string> trackList)
{
	std::deque<NoteEvent> noteEvents;
	
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

	// special sauce for drum notation
	if (track == "drum8") {
		return makeDrumPattern(0.125, events);
	}

	if (track == "drum16") {
		return makeDrumPattern(0.0625, events);
	}


	// now check if the track name is in trackList, otherwise print a warning and return an empty list
	if (std::find(trackList.begin(), trackList.end(), track) == trackList.end()) {
		spdlog::warn("'{}' is not in track list! see 'tracks' for available tracks, drum8 and drum16 are also valid", track);
		return noteEvents;
	}



	// now use `events`
	auto timestamp = 0.;
	
	// catch exception if track isn't found
	
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
			noteEvents.push_back(ev);
		}
	}

	return noteEvents;
}

std::deque<NoteEvent> NoteGenerator::sortTimeVal(std::deque<NoteEvent> notes)
{
	// sort then play
	std::vector<NoteEvent> noteVec;
	while (!notes.empty()) {
		noteVec.push_back(notes.front());
		notes.pop_front();
	}

	std::sort(
		noteVec.begin(),
		noteVec.end(),
		[](const NoteEvent& lhs, const NoteEvent& rhs)
		{ return lhs.timeVal < rhs.timeVal; }
	);

	notes = {};

	for (int i = 0; i < noteVec.size(); i++) {
		notes.push_back(noteVec[i]);
	}

	return notes;

}




std::deque<NoteEvent> NoteGenerator::randomPattern(std::string track, size_t numSteps, Scale scale)
{
	std::deque<NoteEvent> noteEvents;
	
	// now use `events`
	auto timestamp = 0.;
	float minDur = 0.0625;
	float maxDur = 0.250;

	srand((unsigned int)time(NULL));

	for (size_t idx = 0; idx < numSteps; idx++) {
		// split notes into duration and note
		float randDur16ths = (rand() % 4) + 1;
		timestamp += (randDur16ths*minDur);
		// only generate one bar
		if (timestamp >= 1.) {
			break;
		}
		size_t randScaleIdx = rand() % (scale.length() + 1);
		auto ev = makeNote(scale(randScaleIdx), true, timestamp, track);
		noteEvents.push_back(ev);
	}

	return noteEvents;
}

std::deque<NoteEvent> NoteGenerator::scalePattern(Key key, ScalePattern pattern, ScaleMode mode)
{
	(void)key;
	(void)pattern;
	(void)mode;
	std::deque<NoteEvent> noteEvents;
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
		noteEvents.push_back(ev);
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
