#include "constant.h"

Constant::Constant()
	: Node(NodeType::CONSTANT, "constant", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
{
	paramMap = {
			{"node_id", NODE_ID},
	};

	inputMap = {
			{"input1_id", INPUT1_ID},
			{"input2_id", INPUT2_ID},
			{"input3_id", INPUT3_ID},
			{"input4_id", INPUT4_ID},
			{"input1", INPUT1},
			{"input2", INPUT2},
			{"input3", INPUT3},
			{"input4", INPUT4},
	};

	outputMap = {
			{"output1_id", OUTPUT1_ID},
			{"output2_id", OUTPUT2_ID},
			{"output3_id", OUTPUT3_ID},
			{"output4_id", OUTPUT4_ID},
			{"value1", VALUE1},
			{"value2", VALUE2},
			{"value3", VALUE3},
			{"value4", VALUE4},
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

