#ifndef METRONOME_H_
#define METRONOME_H_

class Metronome
{
public:
	Metronome();
	void updateBPM(size_t beatsPerMinute);
	void tick();
	bool isOnBeat() const;

private:

	size_t const secondsPerMinute = 60;
	size_t m_bpm;
	size_t m_samplesPerBeat;
	size_t m_samplesElapsed;
	bool m_bOnBeat;

};

#endif // METRONOME_H_