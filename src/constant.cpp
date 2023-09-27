#include "constant.h"

Constant::Constant()
	: Node(NodeType::CONSTANT, "constant", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
{
	paramMap = {
			{"node_id", NODE_ID},
	};

	inputMap = {
			{"input1", INPUT1},
			{"input2", INPUT2},
			{"input3", INPUT3},
			{"input4", INPUT4},
	};

	outputMap = {
			{"value1", VALUE1},
			{"value2", VALUE2},
			{"value3", VALUE3},
			{"value4", VALUE4},
	};

	inputIdStrings = {
		"input1_id",
		"input2_id",
		"input3_id",
		"input4_id"
	};

	outputIdStrings = {
		"output1_id",
		"output2_id",
		"output3_id",
		"output4_id"
	};

}

float Constant::process()
{
	if (inputs[INPUT1] != 0) {
		outputs[VALUE1] = inputs[INPUT1];
	}

	if (inputs[INPUT2] != 0) {
		outputs[VALUE2] = inputs[INPUT2];
	}

	if (inputs[INPUT3] != 0) {
		outputs[VALUE3] = inputs[INPUT3];
	}

	if (inputs[INPUT4] != 0) {
		outputs[VALUE4] = inputs[INPUT4];
	}

	return 0;
}

