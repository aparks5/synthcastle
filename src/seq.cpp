#include "seq.h"

Seq::Seq()
	: Node(NodeType::PROCESSOR, "seq", NUM_INPUTS, NUM_OUTPUTS + 8, NUM_PARAMS + (2 * maxSteps * maxTracks))
	, step(0)
{
	paramMap = {
			{"node_id", NODE_ID},
			{"step", STEP},
			{"gatemode", GATEMODE},
			{"pattern", PATTERN},
			{"song", SONG},
			{"num_params", NUM_PARAMS} // not standard but needed i think 
	};

	for (size_t track = 0; track < maxTracks; track++) {
		for (size_t step = 0; step < maxSteps; step++) {
			auto str = "step_" + std::to_string(step) + "_" + std::to_string(track);
			paramMap[str] = NUM_PARAMS + (track * maxSteps) + step;
			auto enable_str = "enable_" + std::to_string(step) + "_" + std::to_string(track);
			paramMap[enable_str] = enableOffset + (track * maxSteps) + step;
		}
	}

	inputMap = {
		{"trigin", TRIGIN},
		{"reset", RESET},
	};

	for (size_t track = 1; track <= maxTracks; track++) {
		auto trackStr = "track_" + std::to_string(track);
		outputMap[trackStr] = track - 1;
	}

	initIdStrings();

}

void Seq::process() noexcept
{

	params[STEP] = step;

	if (inputs[RESET] == 1) {
		for (size_t track = 0; track < maxTracks; track++) {
			outputs[track] = 0;
		}
		inputs[RESET] = 0;
		step = 0;
	}

	if (inputs[TRIGIN] != 0) {
		inputs[TRIGIN] = 0;

		for (int track = 0; track < maxTracks; track++) {
			float out = 0.f;
			// gargantuan indexing
			enabled[track][step] = params[enableOffset + (track * maxSteps) + step];
			notes[track][step] = params[NUM_PARAMS + (track * maxSteps) + step];
			if (enabled[track][step] == 1) {
				out = (params[GATEMODE] == 1) ? 1 : notes[track][step];
			}
			else {
				out = 0;
			}
			outputs[track] = out;
		}

		step++;
		if (step >= maxSteps) {
			step = 0;
		}

	}

}

