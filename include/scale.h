#ifndef SCALE_H_
#define SCALE_H_

#include <string>
#include <vector>


class Scale
{
public:
	enum ScalePattern
	{
		MAJOR,
		MINOR,
		HARMONIC_MINOR,
		BLUES,
		CHROMATIC,
		PHRYGIAN_DOMINANT,
		WHOLE_TONE,
	};

	enum ScaleMode
	{
		IONIAN,
		DORIAN,
		PHRYGIAN,
		LYDIAN,
		MIXOLYDIAN,
		AEOLIAN,
		LOCRIAN
	};

	enum Key
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

	Scale(Key key, ScalePattern pattern, ScaleMode mode);
	void transpose(Key key);
	void modulate(ScaleMode mode);
	size_t operator()(size_t idx) { return m_scale[idx]; }
	size_t length() { return m_scale.size() - 1; }
	static Key strToKey(std::string str);
	static ScalePattern strToScalePattern(std::string str);
	static ScaleMode strToScaleMode(std::string str);

private:
	// given the chosen pattern, fill in the chosen scale
	void generate(Key key, ScalePattern pattern);
	// fill in the pattern starting from note start
	void fill(int start, std::vector<int> pattern);
	Key m_key;
	ScalePattern m_pattern;
	ScaleMode m_mode;
	size_t m_lowestOctave;
	size_t m_numOctaveRange;
	std::vector<size_t> m_scale;
	std::vector<size_t> m_steps;
};

#endif // SCALE_H_