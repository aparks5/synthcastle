#include "relay.h"

ProcessorOutput::ProcessorOutput(size_t index)
	: Node(NodeType::PROCESSOR_OUTPUT, "processor_output", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
{
	outputs[OUTPUT] = static_cast<float>(index);
}


