#include "metronome.h"
#include "constants.h"

Metronome::Metronome()
	: m_bpm(60)
	, m_bOnBeat(true)
	, m_samplesElapsed(0)
{
	bpm(m_bpm);
}

void Metronome::bpm(size_t beatsPerMinute)
{
	m_bpm = beatsPerMinute;
	reset();
}

void Metronome::reset()
{
	m_samplesPerBeat = SAMPLE_RATE * secondsPerMinute / m_bpm;
	m_samplesElapsed = 0;
	m_bOnBeat = true;
}

bool Metronome::isOnBeat() const
{
	return m_bOnBeat;
}

void Metronome::tick()
{
	m_samplesElapsed++;

	if (m_samplesElapsed > m_samplesPerBeat) {
		m_samplesElapsed = 0;
		m_bOnBeat = true;
	}
	else {
		m_bOnBeat = false;
	}
}