#ifndef SYNTH_H_
#define SYNTH_H_

#include <vector>
#include <memory>

#include "voice.h"
#include "fx.h"
#include "bitcrusher.h"
#include "chorus.h"
#include "tapedelay.h"
#include "platereverb.h"
#include "pitchshift_timedomain.h"

class Synth : public Module
{
public:
	Synth(size_t fs);
    virtual void noteOn(size_t noteVal) override;
    virtual void noteOff(size_t noteVal) override;
    void update(VoiceParams params);
    void update(FxParams params);
    void blockRateUpdate();
    float operator()() override;
    float operator()(float in) override { return 0.f; }



private:
    std::vector<std::shared_ptr<Voice>> m_voices;
    FxParams m_fxParams;
    Chorus chorus;
    TapeDelay delay;
    Delay delay2;
    Delay delay3;
    Bitcrusher bitcrush;
    PlateReverb reverb;
    PitchShift pitch;
    size_t lastActiveVoice;


};

#endif // SYNTH_H_