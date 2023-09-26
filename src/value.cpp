
#include "value.h"

ProcessorInput::ProcessorInput(float val)
	: Node(NodeType::PROCESSOR_INPUT, "processor_input", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
{
	outputs[OUTPUT] = val;
};

ProcessorInput::ProcessorInput()
	: Node(NodeType::PROCESSOR_INPUT, "processor_input", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
{
	outputs[OUTPUT] = 0.f;
};






