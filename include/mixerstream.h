/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef STREAM_H_ 
#define STREAM_H_ 

#include "portaudio.h"
#include <vector>
#include <thread>
#include "synth.h"
#include "sampler.h"
#include "schroederallpass.h"
#include "comb.h"

#include "commands.h"

class MixerStream
{
public:
    MixerStream(CallbackData* userData);
    bool open(PaDeviceIndex index);
    bool close();
    bool start();
    bool stop();
    void update(VoiceParams params);
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
    std::vector<SchroederAllpass> m_allpassFilters;
    std::vector<Comb> m_combFilters;

};


#endif // STREAM_H_ 

