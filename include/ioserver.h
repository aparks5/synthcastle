#ifndef IOSERVER_H_
#define IOSERVER_H_

#include "constants.h"
#include <iostream>
#include <fstream>

class IOServer
{
public:
	IOServer();
	void openWrite();
	void closeWrite();
	void write(std::array<float,FRAMES_PER_BUFFER> buff);
private:
	std::ofstream m_writeStream;
};

#endif // IOSERVER_H_