#include "buffer.h"

Buffer::Buffer<T>(size_t numSamples, size_t numChannels, size_t sampleRate)
	: m_numSamples(numSamples)
	, m_numChans(numChannels)
	, m_sampleRate(sampleRate)
	, m_storage(numSamples*numChannels)
{
	std::fill(m_storage.begin(), m_storage.end(), 0);

}
