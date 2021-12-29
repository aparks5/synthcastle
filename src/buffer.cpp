#include "buffer.h"

Buffer::Buffer(size_t numChans, size_t numSamples)
{

    std::vector<float> myRow(256, 0.f);
    for (size_t idx = 0; idx < numChans; idx++) {
        m_buff.push_back(myRow);
    }

}
