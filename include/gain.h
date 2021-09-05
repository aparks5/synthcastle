#ifndef GAIN_H_
#define GAIN_H_

class Gain
{
public:
	Gain();
	float apply(float samp);

	void setGaindB(float gaindB);
	void setGainf(float gainf);

private:
	int floatTodB(float f) const;
	float dBtoFloat(int db) const;
	float m_fgain;
};

#endif // GAIN_H_