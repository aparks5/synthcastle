#pragma once
#include "node.h"
#include <array>

class FourVoice : public Node
{
public:
	FourVoice(int voiceId);
	virtual ~FourVoice() {}

	void process() noexcept override;
	void display() override;

	enum Inputs
	{
		NUM_INPUTS
	};

	enum Outputs
	{
		NUM_OUTPUTS
	};

	enum FourVoiceParams
	{
		NODE_ID,
		INPUT,
		INPUT_ID,
		ACTIVE,
		NUM_PARAMS
	};

private:
	int m_voiceId;
	int m_counter;
	int m_cache;

};
