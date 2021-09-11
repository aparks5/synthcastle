/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef VOICE_H_
#define VOICE_H_

#include "envelope.h"
#include "gain.h"
#include "KrajeskiMoog.h"
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

struct VoiceParams
{
    OscillatorType osc;
    float freq;
    float filtFreq;
    float filtQ;
    EnvelopeParams envParams;
    float filtLFOFreq;
    float pitchLFOfreq;
    bool bEnableFiltLFO;
    bool bEnablePitchLFO;
};

class Voice
{
public:
	Voice();
	float apply();
    void update();
    bool active() { return m_bIsActive; }

    void updateFreq(float freq);
    void oscillate(float& output);
    void modFreq(float freq);
    void updateGain(int gaindB);
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
    bool m_bIsActive;
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


#endif // VOICE_H_