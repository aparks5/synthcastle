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

	for (size_t idx = 0; idx < offset; idx++) {
		m_scale.push_back(m_scale[idx] + oct);
	}

	// now remove the elements from the beginning
	m_scale.erase(m_scale.begin(), m_scale.begin() + offset);

}