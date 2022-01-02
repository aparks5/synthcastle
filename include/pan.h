#ifndef PAN_H_
#define PAN_H_

#include "module.h"
#include <array>
#include "sine.h"

class Pan : public Module
{
public:
	Pan(size_t fs);
	float operator()(float in) override;
	float operator()() override { return 0.f; }
	void operator()(std::array<std::array<float, 256>, 2> &outputBuffer);
	void update(float percent);

private:

	Sine m_panLfo;
	float m_percent;
	float m_leftGain;
	float m_rightGain;


};


#endif