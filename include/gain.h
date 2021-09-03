#ifndef GAIN_H_
#define GAIN_H_

class Gain
{
public:
	Gain();
	void setGaindB(int gaindB);
	float apply(float samp);

private:
	int floatTodB(float f) const;
	float dBtoFloat(int db) const;
	float m_fgain;
};

#endif // GAIN_H_