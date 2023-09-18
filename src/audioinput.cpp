#pragma once

#include "node.h"
#include <unordered_map>
#include "audioinput.h"

AudioInput::AudioInput()
	: Node(NodeType::AUDIO_IN, 0.f, NUM_PARAMS)
{}
