#include "scale.h"

Scale::Scale(Key key, ScalePattern pattern, ScaleMode mode)
	: m_key(key)
	, m_pattern(pattern)
	, m_mode(mode)
	, m_lowestOctave(3)
	, m_numOctaveRange(1)
{
    /// calculate notes in scale
	/// convert key to starting note
	generate(key, pattern);
	modulate(mode);
}



 Key Scale::strToKey(std::string str)
{
	Key key = Key::C;
	if (str == "C") {
		key = Key::C;
	}
	else if (str == "C#" || str == "Db") {
		key = Key::CSHARP;
	}
	else if (str == "D") {
		key = Key::D;
	}
	else if (str == "Eb" || str == "D#") {
		key = Key::EFLAT;
	}
	else if (str == "F") {
		key = Key::F;
	}
	else if (str == "F#" || str == "Gb") {
		key = Key::FSHARP;
	}
	else if (str == "G") {
		key == Key::G;
	}
	else if (str == "G#" || str == "Ab") {
		key == Key::AFLAT;
	}
	else if (str == "A") {
		key == Key::A;
	}
	else if (str == "A#" || str == "Bb") {
		key == Key::BFLAT;
	}
	else if (str == "B") {
		key == Key::B;
	}

	return key;

}

 ScalePattern Scale::strToScalePattern(std::string str)
 {
	 ScalePattern pat = ScalePattern::MAJOR;
	 if (str == "maj") {
		 pat = ScalePattern::MAJOR;
	 }
	 else if (str == "min") {
		 pat = ScalePattern::MINOR;
	 }
	 else if (str == "blues") {
		 pat = ScalePattern::BLUES;
	 }

	 return pat;

 }

 ScaleMode Scale::strToScaleMode(std::string str)
 {
	 ScaleMode mode = ScaleMode::IONIAN;

	 if (str == "ionian" || str == "i") {
		 mode = ScaleMode::IONIAN;
	 }
	 else if (str == "dorian" || str == "ii") {
		 mode = ScaleMode::DORIAN;
	 }
	 else if (str == "phrygian" || str == "iii") {
		 mode = ScaleMode::PHRYGIAN;
	 }
	 else if (str == "lydian" || str == "iv") {
		 mode = ScaleMode::LYDIAN;
	 }
	 else if (str == "mixolydian" || str == "v") {
		 mode = ScaleMode::MIXOLYDIAN;
	 }
	 else if (str == "aeolian" || str == "vi") {
		 mode = ScaleMode::AEOLIAN;
	 }
	 else if (str == "locrian" || str == "vii") {
		 mode = ScaleMode::LOCRIAN;
	 }

	 return mode;
 }


void Scale::generate(Key key, ScalePattern pattern)
{
	size_t startingNote = (12 * m_lowestOctave) + static_cast<size_t>(key);
	if (pattern == ScalePattern::MAJOR) {
		size_t nextNote = startingNote;
		m_scale.push_back(nextNote);
		nextNote += 2;
		m_scale.push_back(nextNote);
		nextNote += 2;
		m_scale.push_back(nextNote);
		nextNote += 1;
		m_scale.push_back(nextNote);
		nextNote += 2;
		m_scale.push_back(nextNote);
		nextNote += 2;
		m_scale.push_back(nextNote);
		nextNote += 2;
		m_scale.push_back(nextNote);
		nextNote += 1;
		m_scale.push_back(nextNote);
	}

	if (pattern == ScalePattern::MINOR) {
		size_t nextNote = startingNote;
		m_scale.push_back(nextNote);
		nextNote += 2;
		m_scale.push_back(nextNote);
		nextNote += 1;
		m_scale.push_back(nextNote);
		nextNote += 2;
		m_scale.push_back(nextNote);
		nextNote += 2;
		m_scale.push_back(nextNote);
		nextNote += 1;
		m_scale.push_back(nextNote);
		nextNote += 2;
		m_scale.push_back(nextNote);
		nextNote += 2;
		m_scale.push_back(nextNote);
	}
	if (pattern == ScalePattern::BLUES) {
		size_t nextNote = startingNote;
		m_scale.push_back(nextNote);
		nextNote += 3;
		m_scale.push_back(nextNote);
		nextNote += 2;
		m_scale.push_back(nextNote);
		nextNote += 1;
		m_scale.push_back(nextNote);
		nextNote += 1;
		m_scale.push_back(nextNote);
		nextNote += 3;
		m_scale.push_back(nextNote);
		nextNote += 2;
		m_scale.push_back(nextNote);
	}
}

void Scale::transpose(Key key)
{
	Key oldKey = m_key;
	m_key = key;
	size_t offset = static_cast<size_t>(m_key) - static_cast<size_t>(oldKey);
	for (size_t idx = 0; idx < m_scale.size(); idx++) {
		m_scale[idx] += offset;
	}
}

void Scale::modulate(ScaleMode mode)
{
    // for modes, we rearrange the notes of the scale and move the notes previous to the modes degree up an octave and at the end of the scale
	size_t offset = static_cast<size_t>(mode);
	size_t oct = 12;

	// we don't need to rearrange the first note since we already include the octave in the scale
	for (size_t idx = 1; idx <= offset; idx++) {
		m_scale.push_back(m_scale[idx] + oct);
	}

	// now remove the elements from the beginning
	m_scale.erase(m_scale.begin(), m_scale.begin() + offset);

}