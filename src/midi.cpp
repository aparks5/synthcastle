#include "midi.h"
#include <functional>

MIDI::MIDI(MixerStream& stream)
{
	RtMidiIn* midiin = 0;
	midiin = new RtMidiIn();
	chooseMidiInPort(midiin);

	RtMidiOut* midiout = 0;

	// RtMidiOut constructor
	try {
		midiout = new RtMidiOut();
		chooseMidiPort(midiout);
	}
	catch (RtMidiError& error) {
		error.printMessage();
		exit(EXIT_FAILURE);
	}

	midiUserData.stream = &stream;
	std::queue<NoteEvent> notes;
	midiUserData.notes = notes;

	midiin->setCallback(&MIDI::midiCallback, &midiUserData);
}

void MIDI::midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData)
{
    MIDIUserData* pUserData = (MIDIUserData*)userData;
    MixerStream* stream = pUserData->stream;
	// midi note to freq formula https://newt.phys.unsw.edu.au/jw/notes.html
	double stamp = 0;
	auto nBytes = 0;
	nBytes = message->size();
	if (nBytes == 3) {
		int byte0 = (int)message->at(0);
		auto noteVal = (int)message->at(1);
		float velocity = (int)message->at(2);
		if (byte0 == 144) {
			if (velocity != 0) {
				stream->noteOn(noteVal, 1);
			}
			else {
				stream->noteOff(noteVal, 1);
			}
		}
		else if (byte0 == 128) {
			stream->noteOff(noteVal, 1);
		}
	}
}

bool MIDI::chooseMidiPort(RtMidiOut* rtmidi)
{
	std::cout << "\nWould you like to open a virtual output port? [y/N] ";

	std::string keyHit;
	std::getline(std::cin, keyHit);
	if (keyHit == "y") {
		rtmidi->openVirtualPort();
		return true;
	}

	std::string portName;
	unsigned int i = 0, nPorts = rtmidi->getPortCount();
	if (nPorts == 0) {
		std::cout << "No output ports available!" << std::endl;
		return false;
	}

	if (nPorts == 1) {
		std::cout << "\nOpening " << rtmidi->getPortName() << std::endl;
	}
	else {
		for (i = 0; i < nPorts; i++) {
			portName = rtmidi->getPortName(i);
			std::cout << "  Output port #" << i << ": " << portName << '\n';
		}

		do {
			std::cout << "\nChoose a port number: ";
			std::cin >> i;
		} while (i >= nPorts);
	}

	std::cout << "\n";
	rtmidi->openPort(i);

	return true;
}



bool MIDI::chooseMidiInPort(RtMidiIn* rtmidi)
{
	std::cout << "\nWould you like to open a virtual output port? [y/N] ";

	std::string keyHit;
	std::getline(std::cin, keyHit);
	if (keyHit == "y") {
		rtmidi->openVirtualPort();
		return true;
	}

	std::string portName;
	unsigned int i = 0, nPorts = rtmidi->getPortCount();
	if (nPorts == 0) {
		std::cout << "No output ports available!" << std::endl;
		return false;
	}

	if (nPorts == 1) {
		std::cout << "\nOpening " << rtmidi->getPortName() << std::endl;
	}
	else {
		for (i = 0; i < nPorts; i++) {
			portName = rtmidi->getPortName(i);
			std::cout << "  Output port #" << i << ": " << portName << '\n';
		}

		do {
			std::cout << "\nChoose a port number: ";
			std::cin >> i;
		} while (i >= nPorts);
	}

	std::cout << "\n";
	rtmidi->openPort(i);

	return true;
}





