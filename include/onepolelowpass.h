#ifndef ONEPOLELP_H_
#define ONEPOLELP_H_

class OnePoleLowpass
{
public:
	OnePoleLowpass();
	float operator()(float in);

private:
	float m_damp1;
	float m_damp2;
	float m_delayElement;
};
#endif // ONEPOLELP_H_