#include "pitchshift_timedomain.h"

#include "gain.h"

PitchShift::PitchShift(size_t fs)
	: Module(fs)
	, m_delay()
	, m_delayInv()
	, m_saw(fs)
	, m_env(fs)
	, m_shiftSemitones(7)
	, m_bShiftUp(true)
	, m_windowTime(80)
{ 
	//m_delay.update(0.f, 0.3f);
	//m_delayInv.update(0.f, 0.3f);
	update(m_shiftSemitones);

}

void PitchShift::update(float semitones)
{
	float semi = semitones;
	m_bShiftUp = (semi < 0) ? false : true;
	semi = abs(semi);
	float temp = expf(semi * 0.05776f) - 1;

	temp = temp / (m_windowTime * 0.001); // window
	// todo: saw should handle negative frequencies
	//m_saw.params[Oscillator::FREQ] = temp;
	//m_env.params[Oscillator::FREQ] = temp / 2;
}

float PitchShift::operator()(float in) {

	float shiftUp = (m_bShiftUp) ? -1.f : 1.f;
	float delay1Modulation = shiftUp * m_saw.process();
	// make saw unipolar
	delay1Modulation = (delay1Modulation * 0.5f) + 0.5f;

	// add 90 degree offset, wrap around 1
	float delay2Modulation = 0.5f + delay1Modulation;
	if (delay2Modulation > 1) {
		delay2Modulation -= 1;
	}

	//m_delay.update((delay1Modulation * m_windowTime) + 10, 0.67f);
	//m_delayInv.update((delay2Modulation * m_windowTime) + 10, 0.67f);

	float temp = m_delay.process(in);
	// restrict cosine from -.25 to .25
	float env = cosf(2*M_PI * ((delay1Modulation - 0.5f) * 0.5f));
	temp *= env;
	
	float tempInv = m_delayInv.process(in);
	float envInv = cosf(2*M_PI * ((delay2Modulation - 0.5f) * 0.5f));
	tempInv *= envInv;

	return (temp + tempInv) * 0.707;
}
