/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef TRIANGLE_H_ 
#define TRIANGLE_H_ 

#include "portaudio.h"

class Triangle
{
public:
    Triangle();
    virtual ~Triangle() {};
	void setSampleFreq(double frequency) { m_targetFreq = frequency; }
	void update();
	float generate();

private:
    /// @brief The instance callback, where we have access to every method/variable in object of class Triangle */
	int paCallbackMethod(const void* inputBuffer, void* outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags);
 
    /// @brief This routine will be called by the PortAudio engine when audio is needed.
    /// It may called at interrupt level on some machines so don't do anything
    /// that could mess up the system like calling malloc() or free().
    ///
    static int paCallback(const void* inputBuffer, void* outputBuffer,
        unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
		void* userData);

    void paStreamFinishedMethod();
     /// @brief This routine is called by portaudio when playback is done.
    static void paStreamFinished(void* userData);

    double m_targetFreq;
    float m_output;
    float m_incrementBase;
    float m_increment;

};

#endif // TRIANGLE_H_ 

