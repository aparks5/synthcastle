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
	std::shared_ptr<MixerStream> stream;

};

#endif // PROMPT_H_

