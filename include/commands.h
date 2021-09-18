#ifndef CALLBACK_TYPES_H_
#define CALLBACK_TYPES_H_

#include "thirdparty/readerwriterqueue.h"
#include "ioserver.h"

enum class Commands
{
    START_RECORDING,
    STOP_RECORDING
};


typedef struct
{
	std::shared_ptr<moodycamel::ReaderWriterQueue<Commands>> commandsToAudioCallback;
	std::shared_ptr<moodycamel::ReaderWriterQueue<Commands>> commandsFromAudioCallback;
	std::shared_ptr<IOServer> server;
} CallbackData;

#endif CALLBACK_TYPES_H_
