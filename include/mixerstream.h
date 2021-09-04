/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef STREAM_H_ 
#define STREAM_H_ 

#include "portaudio.h"
#include "metronome.h"
#include "saw.h"
#include "triangle.h"
#include "square.h"
#include "sine.h"
#include "gain.h"
#include "envelope.h"
#include <thread>

enum class Oscillator
{
    SINE,
    SAW,
    TRIANGLE,
    SQUARE
};

class MixerStream
{
public:
    MixerStream();
    bool open(PaDeviceIndex index);
    bool close();
    bool start();
    bool stop();
    void updateFreq(float freq);
    void updateGain(int gaindB);
    void updateBPM(size_t bpm);
    void updateOsc(Oscillator osc);


private:
    /// @brief The instance callback, where we have access to every method/variable in object of class MixerStream */
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

    void processUpdates();

    PaStream* stream;
    std::thread m_gfx;
    Metronome m_metronome;
    size_t durationCounter;

    Oscillator m_osc;
    Saw m_saw;
    Triangle m_tri;
    Square m_square;
    Sine m_sine;

    Gain m_gain;
    Envelope m_env;


};


#endif // STREAM_H_ 

