#ifndef SCALE_H_
#define SCALE_H_

#include <vector>

enum class ScalePattern
{
	MAJOR,
	MINOR,
	BLUES,
	CHROMATIC
};

enum class ScaleMode
{
	IONIAN,
	DORIAN,
	PHRYGIAN,
	LYDIAN,
	MIXOLYDIAN,
	AEOLIAN,
	LOCRIAN
};

enum class Key
{
	C,
	CSHARP,
	D,
	EFLAT,
	E,
	F,
	FSHARP,
	G,
	AFLAT,
	A,
	BFLAT,
	B
};

class Scale
{
public:
	Scale(Key key, ScalePattern pattern, ScaleMode mode);
	void transpose(Key key);
	void modulate(ScaleMode mode);
	size_t operator()(size_t idx) { return m_scale[idx]; }
	size_t length() { return m_scale.size() - 1; }

private:
	void generate(Key key, ScalePattern pattern);
	Key m_key;
	ScalePattern m_pattern;
	ScaleMode m_mode;
	size_t m_lowestOctave;
	size_t m_numOctaveRange;
	std::vector<size_t> m_scale;
	std::vector<size_t> m_steps;
};

#endif // SCALE_H_