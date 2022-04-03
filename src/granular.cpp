#include "granular.h"
#include "util.h"


Granular::Granular(size_t fs)
	: Module(sampleRate)
	, m_accum(0)
	, m_increment(1.)
	, m_rate(1.)
	, m_grainSizeHz(1.)
{
	audioFile.load(path);
	m_fileLenSamps = audioFile.getNumSamplesPerChannel();
	update();
}

void Granular::noteOn(size_t noteVal)
{
	// todo: map noteVal to key tracked pitch and position
	m_pos = 0;
	m_accum = 0;
	m_rate = midiNoteToFreq(noteVal) / 440.;
	// initial file position is determined by filePos, may be scaled by noteVal
	calcParams();

}

void Granular::calcParams()
{
	// use grain size to determine grain length, if its longer than the file truncate to file size
	m_grainLenSamps = m_grainSizeHz / m_fs * m_fileLenSamps;
	if (m_grainLenSamps > m_fileLen) {
		m_grainLenSamps = m_fileLen;
	}

	m_filePosMs = clamp(m_filePosMs, 0, m_fileLenSamps / m_fs * 1000.);
	m_initialPos = m_filePosMs / 1000. * m_fs ;

}

void Granular::sprayPos() {
	// (bipolar random (-1/+1) * spray length)
	m_readPos = m_initialPos + rand() * m_sprayLenMs / 1000. * m_fs;
}

void Sample::noteOff(size_t noteVal);
{
	(void)noteVal;
}

float Granular::operator()()
{
	// accumulating by more or less than 1 sample adjusts pitch
	// playing back at 44.1kHz, advance only 1 sample each time
	m_accum += m_rate * m_increment;


	// for granular, go back to start if we're at end of grain
	if (m_accum > (m_grainLength - 1)) {
		m_accum = m_initialPos;
	}

	int nearest = (int)m_accum;
	float remainder = fmodf(m_accum, nearest);

	if (nearest >= 1) {
		return linearInterpolate(audioFile.samples[0][nearest - 1], audioFile.samples[0][nearest], remainder);
	} 
	else {
		return audioFile.samples[0][0];
	}

}