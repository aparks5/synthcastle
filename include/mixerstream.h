/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef STREAM_H_ 
#define STREAM_H_ 

#include "portaudio.h"
#include <vector>
#include <thread>
#include "synth.h"
#include "sampler.h"
#include "fdn.h"
#include "platereverb.h"

#include "commands.h"

struct MixParams
{
    float m_gain1;
    float m_gain2;
    float m_gain3;
    float m_gain4;
    float m_gain5;
    float m_gain6;
};

class MixerStream
{
public:
    MixerStream(CallbackData* userData);
    bool open(PaDeviceIndex index);
    bool close();
    bool start();
    bool stop();
    void update(VoiceParams params);
    void update(FxParams fxparams);
    void updateTrackGainDB(size_t trackNum, float gainDB);
    void noteOn(int noteVal, int track);
    void noteOff(int noteVal, int track);
    void record(bool bStart);

private:
    /// @brief The instance callback, where we have access to every method/variable in object of class MixerStream */
	int paCallbackMethod(const void* inputBuffer, void* outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void* userData);
 
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
    CallbackData* m_callbackData;
    std::ofstream m_writeStream;
    bool m_bRecording;
    std::thread m_gfx;
    Synth m_synth;
    Synth m_synth2;
    Sampler m_kick;
    Sampler m_hat;
    Sampler m_clap;
    Sampler m_snare;
    FeedbackDelayNetwork m_fdn;
    static const size_t kMaxTracks = 16;
    std::array<Gain,16> m_trackGains;
    PlateReverb m_plate;

};


#endif // STREAM_H_ 

