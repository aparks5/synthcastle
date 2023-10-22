#include "scale.h"
#include <unordered_map>

// generates MIDI notes in the scale typically starting at C1 - MIDI note 24
Scale::Scale(Key key, ScalePattern pattern, ScaleMode mode)
	: m_key(key)
	, m_pattern(pattern)
	, m_mode(mode)
	, m_lowestOctave(2)
	, m_numOctaveRange(1)
{
    /// calculate notes in scale
	/// convert key to starting note
	generate(key, pattern);
	modulate(mode);
}

 Scale::Key Scale::strToKey(std::string str)
{
    std::unordered_map<std::string, Key> keys;
	keys["C"] = Key::C;
	keys["C#"] = Key::CSHARP;
	keys["Db"] = Key::CSHARP;
	keys["D"] = Key::D;
	keys["Eb"] = Key::EFLAT;
	keys["D#"] = Key::EFLAT;
	keys["F"] = Key::F;
	keys["F#"] = Key::FSHARP;
	keys["Gb"] = Key::FSHARP;
	keys["G"] = Key::G;
	keys["G#"] = Key::AFLAT;
	keys["Ab"] = Key::AFLAT;
	keys["A"] = Key::A;
	keys["A#"] = Key::BFLAT;
	keys["Bb"] = Key::BFLAT;
	keys["B"] = Key::B;

	return keys[str];
}

 Scale::ScalePattern Scale::strToScalePattern(std::string str)
 {
	 // TODO: replace with map
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

 Scale::ScaleMode Scale::strToScaleMode(std::string str)
 {
	 ScaleMode mode = ScaleMode::IONIAN;

	 // do we need string control or just indices?
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

    // C1 starts at MIDI note 24 https://www.inspiredacoustics.com/en/MIDI_note_numbers_and_center_frequencies
	size_t start = (12 * m_lowestOctave) + static_cast<size_t>(key);
	
	std::unordered_map<ScalePattern, std::vector<int>> steps;
	steps[ScalePattern::MAJOR] = { 2,2,1,2,2,2,1 };
	steps[ScalePattern::MINOR] = { 2,1,2,2,1,2,2 };
	steps[ScalePattern::HARMONIC_MINOR] = { 2,1,2,2,1,3,1 };
	steps[ScalePattern::BLUES] = { 3,2,1,1,3,2 };
	steps[ScalePattern::CHROMATIC] = { 1,1,1,1,1,1,1,1,1,1,1,1 };
	steps[ScalePattern::PHRYGIAN_DOMINANT] = { 1, 1, 1, 2, 1, 2, 2 };
	steps[ScalePattern::WHOLE_TONE] = { 2,2,2,2,2,2 };
	fill(start, steps[pattern]);
}

void Scale::fill(int startingNote, std::vector<int> pattern)
{
	size_t nextNote = startingNote;
	m_scale.push_back(nextNote);

	for (auto& step : pattern) {
		nextNote += step;
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