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
	std::queue<NoteEvent> notes;
	NoteGenerator gen;
	bool bParamChanged = false;

	while (true) {
		std::cout << ">>> commands: stop, osc, freq, filter-freq, filter-q, filt-lfo-freq, pitch/filt-lfo-on/off, " << std::endl;
		std::cin >> prompt;
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
			auto notes = gen.makeSequence(pattern);
			playPattern(notes);
		}
		if (prompt == "loop") {
			std::string pattern;
			std::vector<std::string> param;
			auto paramCount = 0;
			while (paramCount < 2 && std::cin >> pattern) {
				param.push_back(pattern);
				paramCount++;
			}
			NoteGenerator gen;
			int loopCount = std::stod(param[1]);
			while (loopCount > 0) {
				auto notes = gen.makeSequence(param[0]);
				playPattern(notes);
				loopCount--;
			}
		}


		if (bParamChanged) {
			stream.update(params);
		}
	}
}

void Prompt::playPattern(std::queue<NoteEvent> notes)
{

	int now = 0.f;

	while (!notes.empty()) {
		NoteEvent ev = static_cast<NoteEvent>(notes.front());
		notes.pop();
		double stamp = 0;
		auto nBytes = 0;
		auto message = &ev.message;
		nBytes = message->size();
		if (nBytes == 3) {
			int byte0 = (int)message->at(0);
			auto noteVal = (int)message->at(1);
			float velocity = (int)message->at(2);
			if (byte0 == 144) {
				if (velocity != 0) {
					stream.noteOn(noteVal, ev.track);
				}
			}
			else if (byte0 == 128) {
				stream.noteOff(noteVal, ev.track);
			}
		}
		Sleep(ev.timeVal - now);
	}

}