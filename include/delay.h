#pragma once

#include "node.h"
#include <vector>
#include <unordered_map>
#include "onepolelowpass.h"
#include "onepolehighpass.h"
#include "sine.h"

class Delay : public Node 
{
public:
	Delay();
	virtual ~Delay() {};
	float process(float in) override;

	enum Inputs {
		INPUT_ID,
		NUM_INPUTS
	};

	enum Outputs {
		OUTPUT_ID,
		OUTPUT,
		NUM_OUTPUTS
	};

	enum Params {
		NODE_ID,
		DELAY_MS_ID,
		FEEDBACK_RATIO_ID,
		RESET_ID,
		DELAY_MS,
		DELAY_TYPE, // digital, tape
		MODRATE_HZ,
		MODDEPTH_MS,
		FEEDBACK_HIGHPASS_HZ,
		FEEDBACK_LOWPASS_HZ,
		DRYWET_RATIO,
		FEEDBACK_RATIO,
		RESET,
		NUM_PARAMS
	};

	void update(float delayMs, float feedbackRatio);
	void reset();
	void write(float val);
	float tap(float ms);
	void skip();

private:
	float m_sampleRate;
	float m_maxDelaySeconds;
	float m_delayMs;
	size_t m_delaySamps;
	float m_maxDelaySamps;
	std::vector<float> m_circBuff;
	size_t m_writeIdx;
	size_t m_readIdx;
	size_t m_bufSize;
	float m_feedbackRatio;
	float m_feedbackOut;

	float m_cacheLFORateHz;
	float m_cacheLFODepthMs;
	float m_cacheHPCutoffHz;
	float m_cacheLPCutoffHz;

	// modulation and filtering for tape delay
	Sine m_lfo;
	OnePoleHighpass m_hp;
	OnePoleLowpass m_lp;

};
