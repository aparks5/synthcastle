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

};

#endif // PROMPT_H_

