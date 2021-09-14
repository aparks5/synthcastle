#ifndef SYNTH_H_
#define SYNTH_H_

#include <vector>
#include <memory>

#include "voice.h"
#include "chorus.h"
#include "delay.h"

class Synth
{
public:
	Synth();
    void noteOn(int midiNote);
    void noteOff(int midiNote);
    void update(VoiceParams params);
    void blockRateUpdate();
    float operator()();



private:
    std::vector<std::shared_ptr<Voice>> m_voices;
    Delay delay;
    Delay delay2;
    Delay delay3;
    Chorus chorus;
    size_t lastActiveVoice;

};

#endif // SYNTH_H_