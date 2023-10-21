#ifndef NOTE_H_
#define NOTE_H_

#include <vector>
#include <queue>
#include <string>
#include "scale.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h" // must be included



struct NoteEvent
{
	std::vector<unsigned char> message;
	float timeVal;
	std::string track;
	NoteEvent() : timeVal(0.f), track("") {};
	friend std::ostream& operator<<(std::ostream& os, const NoteEvent& c)
	{
		return os << "[NoteEvent track:" << c.track << ",time:" << c.timeVal << ",midinote:" << +c.message[1] << "]";
	}

};

class NoteGenerator
{
public: 
	NoteGenerator() {}
	NoteEvent makeNote(int noteVal, bool bNoteOn, float timeVal, std::string track);
	std::deque<NoteEvent> loopSequence(std::string input, size_t nTimes, std::vector<std::string> trackList);
	std::deque<NoteEvent> makeSequence(std::string input, std::vector<std::string> trackList);
	std::deque<NoteEvent> makeDrumPattern(float quantization, std::vector<std::string> input);
	std::deque<NoteEvent> randomPattern(std::string track, size_t numSteps, Scale scale);
	static std::deque<NoteEvent> sortTimeVal(std::deque<NoteEvent> notes);

};

#endif // NOTE_H_
