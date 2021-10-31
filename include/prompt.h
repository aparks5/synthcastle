#ifndef PROMPT_H_
#define PROMPT_H_

#include <iostream>

#include "midi.h"
#include "note.h"


class Prompt
{
public:
	Prompt(MixerStream& s);
	void open();
private:
	MixerStream& stream;
	std::queue<NoteEvent> makePattern(std::string pattern);
	void playPattern(std::queue<NoteEvent> notes, size_t bpm);

};

#endif // PROMPT_H_

