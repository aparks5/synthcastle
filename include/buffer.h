#ifndef BUFFER_H_
#define BUFFER_H_


class Buffer
{
public:
	Buffer(size_t numChans, size_t numSamps);

private:
	std::vector<std::vector<float>> m_buff;
	size_t numChans;
	size_t numSamps;
	
};