#include "seq.h"
#include "util.h"
#include "scale.h"

Seq::Seq()
	: Node(NodeType::PROCESSOR, "seq", NUM_INPUTS, NUM_OUTPUTS + 4, NUM_PARAMS + (2 * maxSteps * maxTracks))
	, step(0)
	, randomGenerator(randomDevice())
	, random(0, 100)
	, currentPattern(0)
	, scaleKey(Scale::C)
	, scalePattern(Scale::MAJOR)
	, scaleMode(Scale::IONIAN)
	, scale(scaleKey, scalePattern, scaleMode)
{
	paramMap = {
			{"node_id", NODE_ID},
			{"display_step", DISPLAY_STEP}, 
			{"step", STEP}, 
			{"recall", RECALL},
			{"track", TRACK}, 
			{"display_note", DISPLAY_NOTE},
			{"note", NOTE},
			{"key", KEY},
			{"scale", SCALE},
			{"scale_mode", SCALE_MODE},
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

	memset(noteCache, 0, sizeof(noteCache));

}

void Seq::update()
{
	params[DISPLAY_STEP] = step;
	auto p = static_cast<int>(params[PATTERN]);

	if (p != currentPattern) {
		currentPattern = p;
		memset(noteCache, 0, sizeof(noteCache));
		int iter = 0;
		for (int t = 0; t < maxTracks; t++) {
			for (int s = 0; s < maxSteps; s++) {
				params[enableOffset + (t * maxSteps) + s] = pattern[p].sequence[t][s].enabled;
			}
		}
	}

	int t = static_cast<int>(params[TRACK]);
	int s = static_cast<int>(params[STEP]);

	if (pattern[p].sequence[t][s].enabled != params[enableOffset + (t * maxSteps) + s]) {
		pattern[p].sequence[t][s].enabled = params[enableOffset + (t * maxSteps) + s];
	}

	if (params[RECALL] == 1) {
		params[RECALL] = 0;
		params[NOTE] = pattern[p].sequence[t][s].note;
		params[LENGTH] = pattern[p].sequence[t][s].length;
		params[PROBABILITY] = pattern[p].sequence[t][s].probability;
		params[RETRIG] = pattern[p].sequence[t][s].retrig;
		params[KEY] = pattern[p].sequence[t][s].key;
	}

	if (scaleKey != params[KEY]) {
		auto k = params[KEY];
		scaleKey = static_cast<Scale::Key>(k);
		Scale s(scaleKey, scalePattern, scaleMode);
		scale = s;
	}

	if (scalePattern != params[SCALE]) {
		auto sc = params[SCALE];
		scalePattern = static_cast<Scale::ScalePattern>(sc);
		Scale s(scaleKey, scalePattern, scaleMode);
		scale = s;
	}

	if (scaleMode != params[SCALE_MODE]) {
		auto m = params[SCALE_MODE];
		scaleMode = static_cast<Scale::ScaleMode>(m);
		Scale s(scaleKey, scalePattern, scaleMode);
		scale = s;
	}

	if (pattern[p].sequence[t][s].note != params[NOTE]) {
		pattern[p].sequence[t][s].note = params[NOTE];
	}

	if (pattern[p].sequence[t][s].length != params[LENGTH]) {
		pattern[p].sequence[t][s].length = params[LENGTH];
		pattern[p].sequence[t][s].lengthCounter = clamp(static_cast<int>(pattern[p].sequence[t][s].length / 1000.f * 44100.f), 1, 44100);
	}

	if (pattern[p].sequence[t][s].probability != params[PROBABILITY]) {
		pattern[p].sequence[t][s].probability = params[PROBABILITY];
	}

	if (pattern[p].sequence[t][s].retrig != params[RETRIG]) {
		pattern[p].sequence[t][s].retrig = params[RETRIG];
	}

}

float Seq::randomProbability()
{
	return random(randomGenerator) / 100.f;
}

void Seq::process() noexcept
{

	auto p = static_cast<int>(params[PATTERN]);

	if (inputs[RESET] == 1) {
		for (size_t track = 0; track < maxTracks; track++) {
			outputs[track] = 0;
		}
		inputs[RESET] = 0;
		step = 0;
		return;
	}

	for (size_t track = 0; track <= maxTracks; track++) {
		auto trackGate = (2 * track) + 1;
		if (pattern[p].sequence[track][step].lengthCounter <= 0) {
			outputs[trackGate] = 0;
		}
		else {
			pattern[p].sequence[track][step].lengthCounter--;
		}
	}

	if (inputs[TRIGIN] != 0) {
		inputs[TRIGIN] = 0;

		for (int track = 0; track < maxTracks; track++) {
			auto trackCv = (2 * track);
			auto trackGate = (2 * track) + 1;
			float out = 0.f;
			auto prob = pattern[p].sequence[track][step].probability;
			pattern[p].sequence[track][step].enabled = params[enableOffset + (track * maxSteps) + step];
			if (pattern[p].sequence[track][step].enabled == 1) {
				auto numScaleSteps = scale.length();
				// scale returns a midi note, need to go down
				float note = scale(floor(pattern[p].sequence[track][step].note * numScaleSteps)) / 128.f;
				noteCache[track] = note;
				outputs[trackCv] = note;
				// on trigger, reset lengthCounter
				pattern[p].sequence[track][step].lengthCounter = clamp(static_cast<int>(pattern[p].sequence[track][step].length / 1000.f * 44100.f), 1, 44100);
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
				// if the step is not enabled, output the last played note per track
				outputs[trackCv] = noteCache[track];
			}
		}

		step++;
		if (step >= maxSteps) {
			step = 0;
		}

	}

}

