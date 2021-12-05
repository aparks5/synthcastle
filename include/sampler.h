#ifndef SAMPLER_H_
#define SAMPLER_H_

#include <memory>
#include <string>
#include <vector>
#include "module.h"

#include "thirdparty/AudioFile.h"


class Sampler : public Module
{
public:
	Sampler(size_t sampleRate, std::string path);
	float operator()() override;
	float operator()(float in) override { return 0.f; }
    virtual void noteOn(size_t noteVal) override;
    virtual void noteOff(size_t noteVal) override;

private:
	AudioFile<float> audioFile;
	size_t m_pos;
	std::vector<float> m_samples;
	float m_increment;
};


#endif // SAMPLER_H_