#ifndef CALLBACK_TYPES_H_
#define CALLBACK_TYPES_H_

#include "thirdparty/readerwriterqueue.h"
#include "ioserver.h"

enum class Commands
{
    START_RECORDING,
    STOP_RECORDING,
	START_LOOP,
	STOP_LOOP
};


typedef struct
{
	std::shared_ptr<moodycamel::ReaderWriterQueue<Commands>> commandsToAudioCallback;
	std::shared_ptr<moodycamel::ReaderWriterQueue<Commands>> commandsFromAudioCallback;
	std::shared_ptr<IOServer> server;
	std::vector<std::vector<float>> callbackBuffer;
} CallbackData;

#endif CALLBACK_TYPES_H_
