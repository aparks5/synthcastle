#include "prompt.h"
#include "util.h"
#include "windows.h"

Prompt::Prompt(MixerStream& s)
	: stream(s)
{
}
void Prompt::open()
{
	std::string prompt;
	VoiceParams params;
	FxParams fxparams;
	params.envParams = { 1,250,0,0 };
	stream.update(params);
	std::queue<NoteEvent> notes;
	NoteGenerator gen;
	bool bParamChanged = false;
	bool bFxParamChanged = false;
	std::cout << ">>> type 'help' to list commands" << std::endl;

	while (true) {
		std::cout << ">>> ";
		std::cin >> prompt;
		if (prompt == "help") {
			std::cout << ">>> commands: stop, osc, freq, filt-freq, filt-q, filt-lfo-freq, pitch/filt-lfo-on/off, " << std::endl;
			std::cout << ">>> pitch-lfo-freq, pitch-lfo-depth, reverb-on/off, chorus-on/off, delay-on/off" << std::endl;
			std::cout << ">>> osc2-enable, osc2-coarse, osc2-fine, env [attackMs decayMs susdB]" << std::endl;
			std::cout << ">>> play [note], loop [note-note2-note3:duration,note4:duration2... loopNumTimes]" << std::endl;
		}

		if (prompt == "stop") {
			stream.stop();
		}
		if (prompt == "start") {
			stream.start();
		}
		if (prompt == "exit") {
			break;
		}
		if (prompt == "freq") {
			std::cout << ">> enter frequency in Hz" << std::endl;
			std::cin >> prompt;
			auto freq = std::stof(prompt);
			freq = clamp(freq, 0.f, 10000.f);
			params.freq = freq;
			bParamChanged = true;
		}
		if (prompt == "filt-freq") {
			std::cout << ">> enter filter cutoff frequency in Hz" << std::endl;
			std::cin >> prompt;
			auto freq = std::stof(prompt);
			freq = clamp(freq, 0.f, 10000.f);
			params.filtFreq = freq;
			bParamChanged = true;
		}
		if (prompt == "filt-q") {
			std::cout << ">> enter filter resonance (0 - 10)" << std::endl;
			std::cin >> prompt;
			auto q = std::stof(prompt);
			q = clamp(q, 0.f, 10.f);
			params.filtQ = q;
			bParamChanged = true;
		}
		if (prompt == "filt-lfo-freq") {
			std::cout << ">> enter filter LFO frequency (0 - 40)" << std::endl;
			std::cin >> prompt;
			auto freq = std::stof(prompt);
			freq = clamp(freq, 0.f, 40.f);
			params.filtLFOFreq = freq;
			bParamChanged = true;
		}
		if (prompt == "pitch-lfo-freq") {
			std::cout << ">> enter pitch LFO frequency (0 - 40)" << std::endl;
			std::cin >> prompt;
			auto freq = std::stof(prompt);
			freq = clamp(freq, 0.f, 40.f);
			params.pitchLFOFreq = freq;
			bParamChanged = true;
		}
		if (prompt == "pitch-lfo-depth") {
			std::cout << ">> enter pitch LFO depth (0. - 1.)" << std::endl;
			std::cin >> prompt;
			auto depth = std::stof(prompt);
			depth = clamp(depth, 0.f, 1.f);
			params.pitchLFOdepth = depth;
			bParamChanged = true;
		}

		if (prompt == "filt-lfo-on") {
			params.bEnableFiltLFO = true;
			bParamChanged = true;
		}
		if (prompt == "filt-lfo-off") {
			params.bEnableFiltLFO = false;
			bParamChanged = true;
		}
		if (prompt == "pitch-lfo-on") {
			params.bEnablePitchLFO = true;
			bParamChanged = true;
		}
		if (prompt == "pitch-lfo-off") {
			params.bEnablePitchLFO = false;
			bParamChanged = true;
		}
		if (prompt == "chorus-on") {
			fxparams.bEnableChorus = true;
			bFxParamChanged = true;
		}
		if (prompt == "chorus-off") {
			fxparams.bEnableChorus = false;
			bFxParamChanged = true;
		}
		if (prompt == "delay-on") {
			fxparams.bEnableDelay1 = true;
			bFxParamChanged = true;
		}
		if (prompt == "delay-off") {
			fxparams.bEnableDelay1 = false;
			bFxParamChanged = true;
		}
		if (prompt == "reverb-on") {
			fxparams.bEnableReverb = true;
			bFxParamChanged = true;
		}
		if (prompt == "reverb-off") {
			fxparams.bEnableReverb = false;
			bFxParamChanged = true;
		}

		if (prompt == "record-start") {
			stream.record(true);
		}
		if (prompt == "record-stop") {
			stream.record(false);
		}


		if (prompt == "osc") {
			std::cout << ">> enter sine, saw, tri, square" << std::endl;
			std::cin >> prompt;
			OscillatorType osc = OscillatorType::SINE;
			if (prompt == "sine") {
				osc = OscillatorType::SINE;
			}
			if (prompt == "saw") {
				osc = OscillatorType::SAW;
			}
			if (prompt == "tri") {
				osc = OscillatorType::TRIANGLE;
			}
			if (prompt == "square") {
				osc = OscillatorType::SQUARE;
			}
			params.osc = osc;
			bParamChanged = true;
		}
		if (prompt == "osc2-enable") {
			params.bEnableOsc2 = true;
			bParamChanged = true;
		}

		if (prompt == "osc2-disable") {
			params.bEnableOsc2 = false;
			bParamChanged = true;
		}
		if (prompt == "osc2") {
			std::cout << ">> enter sine, saw, tri, square" << std::endl;
			std::cin >> prompt;
			OscillatorType osc = OscillatorType::SINE;
			if (prompt == "sine") {
				osc = OscillatorType::SINE;
			}
			if (prompt == "saw") {
				osc = OscillatorType::SAW;
			}
			if (prompt == "tri") {
				osc = OscillatorType::TRIANGLE;
			}
			if (prompt == "square") {
				osc = OscillatorType::SQUARE;
			}
			params.osc2 = osc;
			bParamChanged = true;
		}
		if (prompt == "osc2-coarse") {
			std::cin >> prompt;
			auto coarse = std::stof(prompt);
			coarse = clamp(coarse, -24.f, 24.f);
			params.osc2coarse = coarse;
		}
		if (prompt == "osc2-fine") {
			std::cin >> prompt;
			auto fine = std::stof(prompt);
			fine = clamp(fine, -1.f, 1.f);
			params.osc2fine = fine;
			bParamChanged = true;
		}
		if (prompt == "env") {
			std::cout << ">> enter adsr envelope parameters (attack ms, decay ms, sustain dB (< 0), release ms) (e.g. 250 10 -10 500) " << std::endl;
			std::vector<std::string> param;
			size_t paramCount = 0;
			std::string envP;
			while (paramCount < 4 && std::cin >> envP) {
				param.push_back(envP);
				paramCount++;
			}
			size_t attMs = 0;
			size_t decMs = 0;
			int susdB = 0;
			size_t relMs = 0;

			std::sscanf(param[0].c_str(), "%zu", &attMs);
			std::sscanf(param[1].c_str(), "%zu", &decMs);
			std::sscanf(param[2].c_str(), "%d", &susdB);
			std::sscanf(param[3].c_str(), "%zu", &relMs);

			attMs = (attMs > 5000) ? 500 : attMs;
			decMs = (decMs > 5000) ? 500 : decMs;
			susdB = (susdB > 0) ? 0 : susdB;
			relMs = (relMs > 5000) ? 500 : relMs;

			EnvelopeParams env(attMs, decMs, susdB, relMs);
			params.envParams = env;
			bParamChanged = true;
		}
		if (prompt == "play") {
			std::string pattern;
			std::cin >> pattern;
			NoteGenerator gen;
			auto temp = gen.makeSequence(pattern);
			while (!temp.empty()) {
				notes.push(temp.front());
				temp.pop();
			}

		}
		if (prompt == "loop") {
			std::cin >> prompt;
			int loopCount = std::stod(prompt);
			if (notes.size() == 0) {
				std::cout << "nothing to loop, use play to declare a sequence..." << std::endl;
			}
			else {
				while (loopCount > 0) {
					playPattern(notes);
					loopCount--;
				}
			}
		}

		if (prompt == "pattern") {
			playPattern(notes);
		}
		if (prompt == "clear") {
			notes = {};
		}

		if (bParamChanged) {
			stream.update(params);
		}

		if (bFxParamChanged) {
			stream.update(fxparams);
		}
	}
}

static std::queue<NoteEvent> sortTimeVal(std::queue<NoteEvent> notes)
{
	// sort then play
	std::vector<NoteEvent> noteVec;
	while (!notes.empty()) {
		noteVec.push_back(notes.front());
		notes.pop();
	}

	std::sort(
		noteVec.begin(),
		noteVec.end(),
		[](const NoteEvent& lhs, const NoteEvent& rhs)
		{ return lhs.timeVal < rhs.timeVal; }
	);

	notes = {};

	for (int i = 0; i < noteVec.size(); i++) {
		notes.push(noteVec[i]);
	}

	return notes;

}


void Prompt::playPattern(std::queue<NoteEvent> notes)
{

	int now = 0.f;

	auto temp = sortTimeVal(notes);

	NoteEvent ev = static_cast<NoteEvent>(temp.front());

	while (!temp.empty()) {

		ev = static_cast<NoteEvent>(temp.front());
		Sleep(ev.timeVal - now);
		now += ev.timeVal - now;
		temp.pop();
		double stamp = 0;
		auto nBytes = 0;
		auto message = &ev.message;
		nBytes = message->size();
		if (nBytes == 3) {
			int byte0 = (int)message->at(0);
			auto noteVal = (int)message->at(1);
			float velocity = (int)message->at(2);
			if (byte0 == 144) {
				if (noteVal == 0)
				{
					if (ev.timeVal - now > 0) {
						Sleep(ev.timeVal - now);
					}
				}
				else if (velocity != 0) {
					stream.noteOn(noteVal, ev.track);
				}
			}
			else if (byte0 == 128) {
				stream.noteOff(noteVal, ev.track);
			}
		}

		if (ev.timeVal - now < 0) {
			now = ev.timeVal;
		}

	}

}