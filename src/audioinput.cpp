#pragma once

#include "node.h"
#include <unordered_map>
#include "audioinput.h"

AudioInput::AudioInput()
	: Node(NodeType::PROCESSOR, "audio_input", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
{
	paramMap = {
		{"node_id", NODE_ID},
		{"adc_input", ADC_INPUT}
	};

	outputMap = {
		{"output", OUTPUT},
	};

	initIdStrings();

}

void AudioInput::process() noexcept
{
	// see evaluate() in main.cpp,
	// the PortAudio callback needs to know about this node
	// because the input comes from the outside
	outputs[OUTPUT] = params[ADC_INPUT];
}

