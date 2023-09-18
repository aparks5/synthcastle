/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef STREAM_H_ 
#define STREAM_H_ 

#include "portaudio.h"
#include <vector>
#include <thread>
#include <string>
#include "synth.h"
#include "sampler.h"
#include "fdn.h"
#include "platereverb.h"
#include "mixer.h"
#include "note.h"
#include <deque>
#include <mutex>

#include "commands.h"

class MixerStream
{
public:
    MixerStream(size_t fs, CallbackData* userData);
    bool open(PaDeviceIndex index);
    bool close();
    bool start();
    bool stop();
    void update(VoiceParams params);
    void update(FxParams fxparams);
    void updateTrackGainDB(std::string track, float gainDB);
    void noteOn(int noteVal, std::string track);
    void noteOff(int noteVal, std::string track);
    void record(bool bStart);
    std::vector<std::string> getTrackList();
    void playPattern(std::deque<NoteEvent> notes, size_t bpm);
    void queueLoop(size_t numLoops, std::deque<NoteEvent> notes, size_t bpm);
    bool shouldLoop() { return m_bLoop; }
    void stopLoop();
    void loop();
    std::string getActiveTrackName() const { return m_activeTrackName; }
    void setActiveTrackName(std::string trackName) {
        m_activeTrackName = trackName;
    }
    void enableMidiLogging() {
        m_bLogMidi = true;
    }
    void disableMidiLogging() {
        m_bLogMidi = false;
    }

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
    Mixer m_mixer;
    Synth m_synth;
    Synth m_synth2;
    //Sampler m_kick;
    //Sampler m_hat;
    //Sampler m_clap;
    //Sampler m_snare;
    FeedbackDelayNetwork m_fdn;
    PlateReverb m_plate;
    bool m_bLoop;
    std::deque<NoteEvent> m_pattern;
    size_t m_loopTimes;
    size_t m_bpm;
    std::mutex m_mtx;
    bool m_bPendingSynthUpdate;
    VoiceParams m_synthUpdate;
    bool m_bPendingFxUpdate;
    FxParams m_fxUpdate;
    std::string m_activeTrackName;
    bool m_bLogMidi;

};


#endif // STREAM_H_ 

