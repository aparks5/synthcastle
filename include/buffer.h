#ifndef BUFFER_H_ 
#define BUFFER_H_ 

#include <vector>

template <class T>
class Buffer
{
public:
    Buffer<T>(size_t numSamples, size_t numChans, size_t sampleRate);
	T& operator[](size_t sampIdx, size_t chanIdx);
	const T& operator[](size_t sampIdx, size_t chanIdx);
	void write(size_t sampIdx, size_t chanIdx, T val)

private:
	size_t m_numChans;
	size_t m_sampleRate;
	size_t m_numSamples;
	// store deinterleaved
	std::vector<T> m_storage;
};

#endif // AUDIO_BUFFER_H_