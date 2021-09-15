#include "prompt.h"
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
			freq = (freq > 0) ? freq : 0;
			freq = (freq < 10000) ? freq : 10000;
			params.freq = freq;
		}
		if (prompt == "filt-freq") {
			std::cout << ">> enter filter cutoff frequency in Hz" << std::endl;
			std::cin >> prompt;
			auto freq = std::stof(prompt);
			freq = (freq > 0) ? freq : 0;
			freq = (freq < 10000) ? freq : 10000;
			params.filtFreq = freq;
		}
		if (prompt == "filt-q") {
			std::cout << ">> enter filter resonance (0 - 10)" << std::endl;
			std::cin >> prompt;
			auto q = std::stof(prompt);
			q = (q > 0) ? q : 0;
			q = (q < 10000) ? q : 10000;
			params.filtQ = q;
		}
		if (prompt == "filt-lfo-freq") {
			std::cout << ">> enter filter LFO frequency (0 - 40)" << std::endl;
			std::cin >> prompt;
			auto freq = std::stof(prompt);
			freq = (freq > 0) ? freq : 0;
			freq = (freq < 40) ? freq : 40;
			params.filtLFOFreq = freq;
		}
		if (prompt == "filt-lfo-on") {
			params.bEnableFiltLFO = true;
		}
		if (prompt == "filt-lfo-off") {
			params.bEnableFiltLFO = false;
		}
		if (prompt == "pitch-lfo-on") {
			params.bEnablePitchLFO = true;
		}
		if (prompt == "pitch-lfo-off") {
			params.bEnablePitchLFO = false;
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
		}
		if (prompt == "osc2-enable") {
			params.bEnableOsc2 = true;
		}

		if (prompt == "osc2-disable") {
			params.bEnableOsc2 = false;
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
		}
		if (prompt == "osc2-coarse") {
			std::cin >> prompt;
			auto coarse = std::stof(prompt);
			coarse = (coarse < -24.) ? -24 : coarse;
			coarse = (coarse > 24.) ? 24 : coarse;
			params.osc2coarse = coarse;
		}
		if (prompt == "osc2-fine") {
			std::cin >> prompt;
			auto fine = std::stof(prompt);
			fine = (fine < -1.) ? -1. : fine;
			fine = (fine > 1.) ? 1. : fine;
			params.osc2fine = fine;
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
		}
		if (prompt == "play") {
			auto n = 0;
			while (n < 8) {
				n++;
				notes.push(gen.makeNote(60, true, 0., 1));
				notes.push(gen.makeNote(63, true, 0., 1));
				notes.push(gen.makeNote(67, true, 0., 1));

				notes.push(gen.makeNote(44, true, 100., 2));
				notes.push(gen.makeNote(44, false, 100., 2));
				notes.push(gen.makeNote(44, true, 100., 2));
				notes.push(gen.makeNote(44, false, 100., 2));

				notes.push(gen.makeNote(60, false, 200., 1));
				notes.push(gen.makeNote(63, false, 200., 1));
				notes.push(gen.makeNote(67, false, 200., 1));
			}

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
				Sleep(ev.timeVal);
			}
		}
		stream.update(params);
	}
}