#ifndef NOTE_H_
#define NOTE_H_

#include <vector>
#include <queue>
#include <string>
#include "scale.h"

struct NoteEvent
{
	std::vector<unsigned char> message;
	float timeVal;
	int track;
	NoteEvent() : timeVal(0.f), track(1) {};
};


class NoteGenerator
{
public: 
	NoteGenerator() {}
	NoteEvent makeNote(int noteVal, bool bNoteOn, float timeVal, int track);
	std::queue<NoteEvent> loopSequence(std::string input, size_t nTimes);
	std::queue<NoteEvent> makeSequence(std::string input);
	std::queue<NoteEvent> randomPattern(size_t trackNum, size_t numSteps, size_t lowNote, size_t highNote);
	std::queue<NoteEvent> scalePattern(Key key, ScalePattern pattern, ScaleMode mode);

};

#endif // NOTE_H_
