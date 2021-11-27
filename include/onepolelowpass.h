#ifndef ONEPOLELP_H_
#define ONEPOLELP_H_

#include "module.h"

class OnePoleLowpass : public Module
{
public:
	OnePoleLowpass(size_t fs);
	float operator()(float in) override;
	float operator()() override { return 0.f; }
	void reset();
	void update(float damping);

private:
	float m_damp1;
	float m_damp2;
	float m_delayElement;
};
#endif // ONEPOLELP_H_