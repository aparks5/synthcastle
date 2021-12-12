#include "controller.h"

Controller::Controller(std::shared_ptr<MixerStream> s)
	: stream(s)
{

}

void Controller::loop()
{
	while (stream->shouldLoop()) {
		stream->loop();
	}
}