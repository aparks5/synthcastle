#include "module.h"
#include "envelope.h"
#include "sampler.h"

class Granular : public Module
{
public:
	Granular(size_t sampleRate, std::string path);
	virtual ~Granular();
	float operator()() override;
	float operator()(float in) override { return 0.f; }
	virtual void noteOn(size_t noteVal) override;
	virtual void noteOff(size_t noteVal) override; 
	// re-calculate parameter values -- todo: add GranularParams struct to pass from interface
	void calcParams();


private:

	// number of overlapping grains
	size_t m_numGrains;

	// grain size in Hz (0 to 100 Hz)...good values 0 to 1 Hz
	float m_grainSizeHz;
	float m_grainLenSamps;
	// duration of file
	float m_fileLenSamps;
	// initial read position
	float m_initialPos;
	// the duration of spraying - random variation in the read position of the grain
	float m_sprayTimeMs;
	float m_positionLfo;
	float m_positionLfoRate;
	float m_positionLfoDepth;

	// amplitude modulation applied to grains. has the effect of muting certain grains
	float m_fluxusAmount;

	// percent key tracking of pitch
	float m_percentPitchKeyTracking;
	// percent key tracking of file position
	float m_percentPositionKeyTracking;

	float m_startingFilePosition;
	float m_scanningFilePosition;


	// envelope to apply to each grain
	Envelope m_envelope;




};