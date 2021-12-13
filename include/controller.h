#ifndef CONTROLLER_H_
#define CONTROLLER_H_


#include "mixerstream.h"

class Controller
{
public:
	Controller(std::shared_ptr<MixerStream> s);
	void loop();
	void stop();

private:
	std::shared_ptr<MixerStream> stream;
};

#endif