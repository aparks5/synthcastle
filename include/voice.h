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
    bool bIsActive;
    OscillatorType osc;
    OscillatorType osc2;
    bool bEnableOsc2;
    float osc2coarse;
    float osc2fine;
    float osc2gain;
    float freq;
    float filtFreq;
    float filtQ;
    EnvelopeParams envParams;
    float filtLFOFreq;
    float pitchLFOFreq;
    bool bEnableFiltLFO;
    bool bEnablePitchLFO;
    bool bEnableFiltEG;
    bool bEnablePitchEG;
    VoiceParams()
        : bIsActive(false)
        , osc(OscillatorType::SAW)
        , osc2(OscillatorType::SAW)
        , bEnableOsc2(false)
        , osc2coarse(0.f)
        , osc2fine(0.f)
        , osc2gain(0.f)
        , freq(0.f)
        , filtFreq(1200.f)
        , filtQ(0.7f)
        , envParams({1,0,0,1})
        , filtLFOFreq(1.)
        , pitchLFOFreq(1.)
        , bEnableFiltLFO(false)
        , bEnablePitchLFO(false)
        , bEnableFiltEG(false)
        , bEnablePitchEG(false)
    {}
};

class Voice
{
public:
	Voice();
	float apply();
    void update(VoiceParams params);
    void modUpdate();
    bool active() { return m_params.bIsActive; }

    void updateFreq(float freq);
    void oscillate(float& output);
    void modFreq(float freq);
    void modFreqOsc2(float freq);
    void noteOn();
    void noteOff();

private:

    VoiceParams m_params;
    OscillatorType m_osc;
    Saw m_saw;
    Triangle m_tri;
    Square m_square;
    Sine m_sine;
    Triangle m_lfo;

    OscillatorType m_osc2;
    Gain m_osc2gain;
    Saw m_saw2;
    Triangle m_tri2;
    Square m_square2;
    Sine m_sine2;

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