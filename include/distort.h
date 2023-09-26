#pragma once

#include "node.h"
#include <unordered_map>

#include "onepolehighpass.h"

class Distort : public Node
{
public:
	Distort();
	virtual ~Distort() {};
	float process(float in) override;

	enum Inputs {
		NUM_INPUTS
	};

	enum Outputs {
		NUM_OUTPUTS
	};

	enum Params {
		NODE_ID,
		INPUT_ID,
		DRIVE_DB, // amount of gain to feed into waveshaper
		ATTEN_DB, // post distortion attenuation in db
		PREEMPH_CUTOFF, // pre-emphasis filter cutoff
		DRYWET,
		ALGORITHM,
		NUM_PARAMS
	};
	
	enum DistortAlgos {
		TANH,
		ATAN,
		SIN,
		TWOSTAGE_SOFTCLIP, // https://www.dafx12.york.ac.uk/papers/dafx12_submission_45.pdf
		CUBIC_SOFTCLIP, 
	};

private:
	OnePoleHighpass m_hp;
	float m_hpCutoffHz;

};
