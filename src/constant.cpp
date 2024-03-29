#include "constant.h"

Constant::Constant()
	: Node(NodeType::PROCESSOR, "constant", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
{
	paramMap = {
			{"node_id", NODE_ID},
			{"value1", VALUE1},
			{"value2", VALUE2},
			{"value3", VALUE3},
			{"value4", VALUE4},
	};

	inputMap = {
			{"input1", INPUT1},
			{"input2", INPUT2},
			{"input3", INPUT3},
			{"input4", INPUT4},
	};

	outputMap = {
			{"output1", OUTPUT1},
			{"output2", OUTPUT2},
			{"output3", OUTPUT3},
			{"output4", OUTPUT4},
	};

	initIdStrings();
}

void Constant::process() noexcept
{
	if (inputs[INPUT1] != 0) {
		params[VALUE1] = inputs[INPUT1];
	}

	if (inputs[INPUT2] != 0) {
		params[VALUE2] = inputs[INPUT2];
	}

	if (inputs[INPUT3] != 0) {
		params[VALUE3] = inputs[INPUT3];
	}

	if (inputs[INPUT4] != 0) {
		params[VALUE4] = inputs[INPUT4];
	}

	outputs[OUTPUT1] = params[VALUE1];
	outputs[OUTPUT2] = params[VALUE2];
	outputs[OUTPUT3] = params[VALUE3];
	outputs[OUTPUT4] = params[VALUE4];

}

