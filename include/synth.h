#ifndef SYNTH_H_
#define SYNTH_H_

#include <vector>
#include <memory>

#include "voice.h"
#include "fx.h"
#include "bitcrusher.h"
#include "chorus.h"
#include "delay.h"
#include "platereverb.h"

class Synth : public Module
{
public:
	Synth(size_t fs);
    void noteOn(int midiNote);
    void noteOff(int midiNote);
    void update(VoiceParams params);
    void update(FxParams params);
    void blockRateUpdate();
    float operator()() override;
    float operator()(float in) override { return 0.f; }



private:
    std::vector<std::shared_ptr<Voice>> m_voices;
    FxParams m_fxParams;
    Chorus chorus;
    Delay delay;
    Delay delay2;
    Delay delay3;
    Bitcrusher bitcrush;
    PlateReverb reverb;
    size_t lastActiveVoice;


};

#endif // SYNTH_H_