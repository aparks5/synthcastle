#include "pitchshift_timedomain.h"

#include "gain.h"

PitchShift::PitchShift(size_t fs)
	: Module(fs)
	, m_delay(fs, 1.0f)
	, m_delayInv(fs, 1.f)
	, m_saw(fs)
	, m_env(fs)
	, m_shiftSemitones(7)
	, m_prevOut(0.)
{ 
	m_delay.update(0.f, 0.0f);
	m_delayInv.update(0.f, 0.0f);
	// http://msp.ucsd.edu/techniques/v0.11/book-html/node125.html
	float semi = m_shiftSemitones;
	float temp = expf(semi * 0.05776f) - 1;
	temp = temp / 0.08; // window
	m_saw.freq(temp);
	m_env.freq(temp / 2);

}


float PitchShift::operator()(float in) {

	// http://msp.ucsd.edu/techniques/v0.11/book-html/node125.html
	float delay1Modulation = m_saw();
	// make saw unipolar
	delay1Modulation = (delay1Modulation * 0.5f) + 0.5f;

	// add 90 degree offset, wrap around 1
	float delay2Modulation = 0.5f + delay1Modulation;
	if (delay2Modulation > 1) {
		delay2Modulation -= 1;
	}

	m_delay.update((delay1Modulation * 80) + 67, 0.0f);
	m_delayInv.update((delay2Modulation * 80) + 67, 0.0f);

	float temp = m_delay();
	// restrict cosine from -.25 to .25
	float env = cosf(2*M_PI * ((delay1Modulation - 0.5f) * 0.5f));
	temp *= env;
	
	float tempInv = m_delayInv();
	float envInv = cosf(2*M_PI * ((delay2Modulation - 0.5f) * 0.5f));
	tempInv *= envInv;

	m_delay.write(in);
	m_delayInv.write(in);
	return (temp + tempInv) * 0.707;
}
