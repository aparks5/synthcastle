/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef SAW_H_ 
#define SAW_H_

class Saw
{
public:
    Saw();
    void setFrequency(float frequency) { m_targetFreq = frequency; }
    /// @brief uppdate increment based on target frequency
    void update();
    /// @brief output the next sample
    float generate();

private:
    float m_targetFreq;
    float m_output;
    float m_increment;
};

#endif // SAW_H_

