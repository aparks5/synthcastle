#pragma once

#include "node.h"
#include <unordered_map>
#include "audioinput.h"

AudioInput::AudioInput()
	: Node(NodeType::AUDIO_IN, "audio_input", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
{
	paramMap = {
		{"node_id", NODE_ID}
	};

	outputMap = {
		{"output_id", OUTPUT_ID},
		{"output", OUTPUT},
	};

}

