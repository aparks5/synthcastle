#include "delay.h"
#include "util.h"

#include <vector>

Delay::Delay()
	: Node(DELAY, "delay", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
	, m_sampleRate(44100)
	, m_maxDelaySeconds(3.0)
	, m_delayMs(0)
	, m_delaySamps(0)
	, m_maxDelaySamps(m_maxDelaySeconds*m_sampleRate)
	, m_writeIdx(0)
	, m_readIdx(0)
	, m_feedbackRatio(0.f)
	, m_feedbackOut(0.f)
	, m_lfo(m_sampleRate)
	, m_hp(m_sampleRate)
	, m_lp(m_sampleRate)
    , m_cacheLFORateHz(0.1)
	, m_cacheLFODepthMs(2)
	, m_cacheHPCutoffHz(1000)
	, m_cacheLPCutoffHz(0.7)
{
	m_hp.update(m_cacheHPCutoffHz);
	m_lp.update(m_cacheLPCutoffHz);
	m_lfo.update(m_cacheLFORateHz); // todo ^ all the above should be user assignable
	// allocate circular buffer
	m_circBuff.resize(static_cast<size_t>(m_maxDelaySamps));
	std::fill(m_circBuff.begin(), m_circBuff.end(), 0.f);
	m_bufSize = m_circBuff.capacity() - 1;

	paramMap = {
		{"node_id", NODE_ID},
		{"input_id", INPUT_ID},
		{"delay_ms", DELAY_MS},
		{"delay_ms_id", DELAY_MS_ID},
		{"modrate_hz", MODRATE_HZ},
		{"moddepth_ms", MODDEPTH_MS},
		{"feedback_highpass_hz", FEEDBACK_HIGHPASS_HZ},
		{"feedback_lowpass_hz", FEEDBACK_LOWPASS_HZ},
		{"drywet_ratio", DRYWET_RATIO},
		{"feedback_ratio", FEEDBACK_RATIO},
		{"feedback_ratio_id", FEEDBACK_RATIO_ID},
		{"reset", RESET},
		{"reset_id", RESET_ID},
		{"delay_type", DELAY_TYPE}
	};


}

float Delay::tap(float ms)
{

	int integerDelay = (int)(ms / 1000.f * m_sampleRate);

	if (static_cast<int>(m_writeIdx) - integerDelay < 0) {
		return m_circBuff[m_bufSize + 1 + (m_writeIdx - integerDelay)];
	}
	else {
		return m_circBuff[m_writeIdx - static_cast<int>(integerDelay)];
	}

}

void Delay::reset()
{
	std::fill(m_circBuff.begin(), m_circBuff.end(), 0.f);
}

float Delay::process(float in) 
{

	// update params if needed
	if (params[MODRATE_HZ] != m_cacheLFORateHz) {
		m_cacheLFORateHz = params[MODRATE_HZ];
		m_lfo.update(m_cacheLFORateHz);
	}

	if (params[MODDEPTH_MS] != m_cacheLFODepthMs) {
		m_cacheLFODepthMs = params[MODDEPTH_MS];
	}

	if (params[FEEDBACK_HIGHPASS_HZ] != m_cacheHPCutoffHz) {
		m_cacheHPCutoffHz = params[FEEDBACK_HIGHPASS_HZ];
		m_hp.update(m_cacheHPCutoffHz);
	}

	if (params[FEEDBACK_LOWPASS_HZ] != m_cacheLPCutoffHz) {
		m_cacheLPCutoffHz = params[FEEDBACK_LOWPASS_HZ];
		m_lp.update(m_cacheLPCutoffHz);
	}

	m_delayMs = params[DELAY_MS] + (m_lfo.process() * m_cacheLFODepthMs); // < delay time depth
	m_feedbackRatio = params[FEEDBACK_RATIO];
	float drywet = params[DRYWET_RATIO];

	// read from buffer

	// find delay index and separate fractional delay for interpolation
	float fractionalDelay = (m_delayMs / 1000.f * m_sampleRate) - (int)(m_delayMs / 1000.f * m_sampleRate);
	int integerDelay = (int)(m_delayMs / 1000.f * m_sampleRate);

	if (static_cast<int>(m_writeIdx) - integerDelay < 0) {
		m_readIdx = m_bufSize + 1 + (m_writeIdx - integerDelay); 
	}
	else {
		m_readIdx = m_writeIdx - static_cast<int>(integerDelay);
	}

	if (static_cast<size_t>(m_delayMs / 1000.f * m_sampleRate) == 0) {
		return m_circBuff[m_writeIdx];
	}

	// for 0 delay, interpolate the input with the prev output
	auto yn = 0.f;

	if ((m_writeIdx == m_readIdx) && integerDelay < 1.) {
		yn = m_circBuff[m_writeIdx];
	}
	else {
		yn = m_circBuff[m_readIdx];
	}

	// find previous delay
	int prevReadIdx = static_cast<int>(m_readIdx) - 1;
	if (prevReadIdx < 0) {
		prevReadIdx = static_cast<int>(m_bufSize); 
	}

	auto yn1 = m_circBuff[prevReadIdx];

	float out = linearInterpolate(yn, yn1, fractionalDelay);
	m_feedbackOut = out;


	// todo: add switch-case for digital/tape/mod/reverse
	float dry = in;
	in = m_hp(in);
	in = m_lp(in);
	in = tanh(in);
	// todo: modulate delay time based on character or params


	// lastly, write to buffer
	m_circBuff[m_writeIdx] = in + (m_feedbackRatio * m_feedbackOut);

	m_writeIdx++;
	if (m_writeIdx > m_bufSize) {
		m_writeIdx = 0;
	}


	return (0.707 * ((drywet * dry) + ((1 - drywet) * out)));
}
