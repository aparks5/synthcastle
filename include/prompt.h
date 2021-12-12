#ifndef PROMPT_H_
#define PROMPT_H_

#include <iostream>
#include <thread>

#include "midi.h"
#include "note.h"


class Prompt
{
public:
	Prompt(std::shared_ptr<MixerStream> s);
	void open();
private:
	std::thread m_audioThread;
	std::shared_ptr<MixerStream> stream;
	void playPattern(std::deque<NoteEvent> notes, size_t bpm);

};

#endif // PROMPT_H_

