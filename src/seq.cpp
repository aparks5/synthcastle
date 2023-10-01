#include "seq.h"

Seq::Seq()
	: Node(NodeType::PROCESSOR, "seq", NUM_INPUTS, NUM_OUTPUTS + 8, NUM_PARAMS + (2 * maxSteps * maxTracks))
	, step(0)
	, randomGenerator(randomDevice())
	, random(0,100)
{
	paramMap = {
			{"node_id", NODE_ID},
			{"display_step", DISPLAY_STEP}, 
			{"step", STEP}, 
			{"recall", RECALL},
			{"track", TRACK}, 
			{"display_note", DISPLAY_NOTE},
			{"note", NOTE},
			{"probability", PROBABILITY},
			{"length", LENGTH},
			{"pattern", PATTERN},
			{"retrig", RETRIG},
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

	int outIdx = 0;
	for (int track = 1; track <= maxTracks; track++) {
		auto cvStr = "cv_" + std::to_string(track);
		auto gateStr = "gate_" + std::to_string(track);
		outputMap[cvStr] = outIdx++;
		outputMap[gateStr] = outIdx++;
	}

	initIdStrings();

}

void Seq::update()
{
	params[DISPLAY_STEP] = step;

	int t = static_cast<int>(params[TRACK]);
	int s = static_cast<int>(params[STEP]);

	// parameter recall
	if (params[RECALL] == 1) {
		params[RECALL] = 0;
		params[NOTE] = sequence[t][s].note * 128.f;
		params[LENGTH] = sequence[t][s].length;
		params[PROBABILITY] = sequence[t][s].probability;
		params[RETRIG] = sequence[t][s].retrig;
	}

	// TODO: watch out for any bugs identical params across steps
	// workaround can be twiddling the knob
	if (sequence[t][s].note != params[NOTE]) {
		sequence[t][s].note = params[NOTE] / 128.f;
	}

	if (sequence[t][s].length != params[LENGTH]) {
		sequence[t][s].length = params[LENGTH];
	}

	if (sequence[t][s].probability != params[PROBABILITY]) {
		sequence[t][s].probability = params[PROBABILITY];
	}

	if (sequence[t][s].retrig != params[RETRIG]) {
		sequence[t][s].retrig = params[RETRIG];
	}


}

float Seq::randomProbability()
{
	return random(randomGenerator) / 100.f;
}

void Seq::process() noexcept
{
	update();

	//for (int out = 0; out < NUM_OUTPUTS; out++) {
	//	outputs[out] = 0;
	//}

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
			auto trackCv = (2 * track);
			auto trackGate = (2 * track) + 1;
			float out = 0.f;
			sequence[track][step].enabled = params[enableOffset + (track * maxSteps) + step];
			outputs[trackCv] = sequence[track][step].note;
			auto prob = sequence[track][step].probability;
			if (sequence[track][step].enabled == 1) {
				if (prob == 0) { // assume we want to trigger
					outputs[trackGate] = 1;
				}
				else if ((randomProbability() < prob)) {
					outputs[trackGate] = 1;
				}
				else {
					outputs[trackGate] = 0;
				}
			}
			else {
				outputs[trackGate] = 0;
			}
		}

		step++;
		if (step >= maxSteps) {
			step = 0;
		}

	}

}

