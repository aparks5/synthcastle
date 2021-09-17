#ifndef FX_H_
#define FX_H_

struct FxParams
{
    float chorusDepth;
    float chorusRate;
    float delay1time;
    float delay1level;
    float delay1feedback;
    float delay2time;
    float delay2level;
    float delay2feedback;
    FxParams() 
        : chorusDepth(0.8f)
        , chorusRate(0.3f)
        , delay1time(250.f)
        , delay1level(0.5f)
        , delay1feedback(0.0)
        , delay2time(250.f)
        , delay2level(0.5f)
        , delay2feedback(0.0)
    {}
};

#endif // FX_H_
