/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef STREAM_H_ 
#define STREAM_H_ 

#include "portaudio.h"

#include <thread>

#include "envelope.h"
#include "gain.h"
#include "KrajeskiMoog.h"
#include "metronome.h"
#include "saw.h"
#include "square.h"
#include "sine.h"
#include "triangle.h"

enum class OscillatorType
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
    void modFreq(float freq);
    void updateGain(int gaindB);
    void updateBPM(size_t bpm);
    void updateOsc(OscillatorType osc);
    void updateEnv(EnvelopeParams params);
    void enableFiltLFO();
    void disableFiltLFO();

    void processUpdates();
    void noteOn();
    void noteOff();
    void updateLfoRate(double freq);
    void updateFilterCutoff(double freq);
    void updateFilterResonance(double q);
    void enablePitchLFO() { m_bEnablePitchLFO = true; }
    void disablePitchLFO() { m_bEnablePitchLFO = false; }


private:
    /// @brief The instance callback, where we have access to every method/variable in object of class MixerStream */
	int paCallbackMethod(const void* inputBuffer, void* outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags);

    void oscillate(float& output);
 
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


    PaStream* stream;
    std::thread m_gfx;
    Metronome m_metronome;
    size_t durationCounter;
    float m_freq;

    OscillatorType m_osc;
    Saw m_saw;
    Saw m_saw2;
    Triangle m_tri;
    Square m_square;
    Sine m_sine;
    Triangle m_lfo;
    bool m_bEnableFilterLFO;
    bool m_bEnablePitchLFO;

    Gain m_gain;
    Envelope m_env;
    Envelope m_env1;
    float m_env1out;
    EnvelopeParams m_envParams;
    KrajeskiMoog m_moogFilter;
    float m_filtFreq;
    bool m_bParamChanged;


};


#endif // STREAM_H_ 

