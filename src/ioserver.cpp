#include "ioserver.h"
#include <array>

IOServer::IOServer()
{
}

void IOServer::openWrite()
{
	m_writeStream.open("audio.raw", std::ios::out | std::ios::binary);
}

void IOServer::closeWrite()
{
	m_writeStream.close();
}

void IOServer::write(std::array<float,FRAMES_PER_BUFFER> buff)
{
	m_writeStream.write(reinterpret_cast<const char*>(buff.data()), std::streamsize(buff.size() * sizeof(float)));

}
