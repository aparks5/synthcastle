#ifndef MIDI_H_
#define MIDI_H_

#include "rtmidi/RtMidi.h"
#include "node.h"
#include <string>
#include <queue>

#include "note.h"

class MIDI : public Node
{
public:
	MIDI();
	virtual ~MIDI() {}
	void process() noexcept override;

	struct MIDIUserData
	{
		int note;
		int velocity;
		MIDIUserData()
			: note(0)
			, velocity(0)
		{}
	};

	enum Inputs
	{
		NUM_INPUTS
	};

	enum Outputs
	{
		OUT_VOICE1,
		OUT_VOICE2,
		OUT_VOICE3,
		OUT_VOICE4,
		OUT_VELOCITY,
		NUM_OUTPUTS
	};

	enum Params
	{
		NODE_ID,
		NUM_PORTS,
		SELECTED_PORT,
		OUT_VOICE1_ID,
		OUT_VOICE2_ID,
		OUT_VOICE3_ID,
		OUT_VOICE4_ID,
		OUT_VELOCITY_ID,
	    NOTE,
		VELOCITY,
		NUM_PARAMS
	};

    MIDIUserData midiUserData;

private:
	std::shared_ptr<RtMidiIn> midiin;
	std::vector<std::string> ports;
	float m_portId;
	static void midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData);
	std::vector<float> m_voices;
	size_t m_currentVoice;
	size_t m_numVoices;
	// to throttle updates that would resize the number of voices
	size_t m_cachedNumVoices;

	int lastActiveVoice;
	float prevFreq;


};


#endif // MIDI_H_
