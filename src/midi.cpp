#include "midi.h"
#include <functional>
#include <iostream>
#include "imnodes.h"

MIDI::MIDI()
	: Node(NodeType::MIDI_IN, 0.f, NUM_PARAMS)
	, midiin(std::make_shared<RtMidiIn>())
{
	midiin->setCallback(&MIDI::midiCallback, &midiUserData);

	// populate the MIDI port list at initialization
	unsigned int nPorts = midiin->getPortCount();
	if (nPorts == 0) {
		// todo: log
		std::cout << "No output ports available!" << std::endl;
	}
	else {
		for (unsigned int i = 0; i < nPorts; i++) {
			ports.push_back(midiin->getPortName(i));
		}
	}

}

void MIDI::midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData)
{
    MIDIUserData* pUserData = (MIDIUserData*)userData;
	// midi note to freq formula https://newt.phys.unsw.edu.au/jw/notes.html
	double stamp = 0;
	auto nBytes = 0;
	nBytes = message->size();
	if (nBytes == 3) {
		int byte0 = (int)message->at(0);
		auto note = (int)message->at(1);
		int velocity = (int)message->at(2);
		pUserData->velocity = velocity;
		if (byte0 == 144) {
			if (velocity != 0) {
				pUserData->note = note;
				printf("note on: %d\n", pUserData->note);
			}
			else {
				pUserData->note = 0;
				printf("note off: %d\n", pUserData->note);
			}
		}
		else if (byte0 == 128) {
			pUserData->note = 0;
			printf("note off: %d\n", pUserData->note);
		}
	}
}

void MIDI::display()
{
	ImNodes::BeginNode(params[MidiParams::NODE_ID]);
	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Midi In");
	ImNodes::EndNodeTitleBar();


	int currentIdx = 0;

	ImGui::PushItemWidth(120.0f);
	if (ports.size() && ImGui::BeginCombo("Port Selection", ports[params[MidiParams::SELECTED_PORT]].c_str(), 0)) {
		for (int n = 0; n < ports.size(); n++) {
			const bool bSelected = (currentIdx == n);
			if (ImGui::Selectable(ports[n].c_str(), bSelected)) {
				currentIdx = n;
				if (currentIdx != params[MidiParams::SELECTED_PORT]) {
					params[MidiParams::SELECTED_PORT] = currentIdx;
					midiin->closePort();
					midiin->openPort(params[MidiParams::SELECTED_PORT]);
				}
				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (bSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	ImGui::PushItemWidth(120.0f);
	ImGui::Spacing();
	ImGui::Text("Note: %d", midiUserData.note);
	ImGui::Text("Velocity: %d", midiUserData.velocity);
	ImGui::PopItemWidth();

	ImGui::Spacing();
	// display note and velocity
	ImNodes::BeginOutputAttribute(params[MidiParams::NODE_ID]);
	ImGui::TextUnformatted("CV Out");
	ImNodes::EndOutputAttribute();
	ImNodes::EndNode();

}

float MIDI::process()
{
	params[MidiParams::NOTE] = midiUserData.note;
	// should return 0 to 1 (0 freq should output 0)
	return (1.f * params[MidiParams::NOTE]) / 128.f;

}

