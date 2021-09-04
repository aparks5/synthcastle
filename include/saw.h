/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef SAW_H_ 
#define SAW_H_

class Saw
{
public:
    Saw();
    void freq(float frequency) { m_freq = frequency; }
    /// @brief update increment based on target frequency
    void update();
    /// @brief output the next sample
    float generate();

private:
    float m_freq;
    float m_output;
    float m_increment;
};

#endif // SAW_H_

