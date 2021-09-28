#ifndef FX_H_
#define FX_H_

struct FxParams
{
    bool bEnableReverb;
	bool bEnableChorus;
    float chorusDepth;
    float chorusRate;
    bool bEnableDelay1;
    float delay1time;
    float delay1level;
    float delay1feedback;
    bool bEnableDelay2;
    float delay2time;
    float delay2level;
    float delay2feedback;
    bool bEnableBitcrusher;
    float bitCrusherNBits;
    FxParams() 
        : bEnableReverb(false)
        , bEnableChorus(false)
        , chorusDepth(0.8f)
        , chorusRate(0.3f)
        , bEnableDelay1(false)
        , delay1time(250.f)
        , delay1level(0.5f)
        , delay1feedback(0.0)
        , bEnableDelay2(false)
        , delay2time(250.f)
        , delay2level(0.5f)
        , delay2feedback(0.0)
        , bEnableBitcrusher(false)
        , bitCrusherNBits(32)
    {}
};

#endif // FX_H_
