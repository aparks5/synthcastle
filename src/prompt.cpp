#include "prompt.h"
#include "util.h"
#include "windows.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"


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

	std::vector<spdlog::sink_ptr> sinks;
	sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_st>());
	sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>("history.log",1024*1024,5,false));
	auto logger = std::make_shared<spdlog::logger>("logger", begin(sinks), end(sinks));
	//register it if you need to access it globally
	spdlog::register_logger(logger);
	spdlog::set_default_logger(logger);
	spdlog::flush_on(spdlog::level::info);

	spdlog::info("opening history log...");

	while (true) {
		std::cout << ">>> ";
		std::cin >> prompt;
		if (prompt == "help") {
			spdlog::info(">>> commands: stop, tracks, mix, osc, freq, filt-freq, filt-q, filt-lfo-freq, pitch/filt-lfo-on/off, ");
			std::cout << ">>> pitch-lfo-freq, pitch-lfo-depth, reverb-on/off, chorus-on/off, delay-on/off" << std::endl;
			std::cout << ">>> delay-time, delay-feedback, delay-mix" << std::endl;
			std::cout << ">>> osc2-enable, osc2-coarse, osc2-fine, env [attackMs decayMs susdB]" << std::endl;
			std::cout << ">>> play [note] [note-note2-note3:duration,note4:duration2...], loop loopNumTimes" << std::endl;
		}

		if (prompt == "stop") {
			stream.stop();
		}
		if (prompt == "start") {
			stream.start();
		}
		if (prompt == "tracks") {
			spdlog::info(">> > list of tracks");
			size_t trackCount = 0;

			std::vector<std::string> trackList = stream.getTrackList();

			for (auto track : trackList) {
				std::cout << trackCount << ": " << track << std::endl;
				trackCount++;
			}


		}

		if (prompt == "bpm") {
			std::cout << ">> enter bpm (20-200 beats per minute)" << std::endl;
			std::cin >> prompt;
			auto bpm = std::stof(prompt);
			bpm = clamp(bpm, 20.f, 200.f);
			params.bpm = bpm;
			bParamChanged = true;
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
		if (prompt == "delay-time") {
			std::cout << ">> enter delay time in milliseconds (0. - 1000.)" << std::endl;
			std::cin >> prompt;
			auto delayTimeMs = std::stof(prompt);
			delayTimeMs = clamp(delayTimeMs, 0.f, 1000.f);
			fxparams.delay1time = delayTimeMs;
			bParamChanged = true;
		}
		if (prompt == "delay-feedback") {
			std::cout << ">> enter delay feedback as a ratio (0. - 1.)" << std::endl;
			std::cin >> prompt;
			auto delayFeedbackRatio = std::stof(prompt);
			delayFeedbackRatio = clamp(delayFeedbackRatio, 0.f, 1.f);
			fxparams.delay1feedback = delayFeedbackRatio;
			bParamChanged = true;
		}
		if (prompt == "delay-mix") {
			std::cout << ">> enter delay wet/dry mix as a ratio (0. - 1.)" << std::endl;
			std::cin >> prompt;
			auto delayMix = std::stof(prompt);
			delayMix = clamp(delayMix, 0.f, 1.f);
			fxparams.delay1level = delayMix;
			bParamChanged = true;
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
		if (prompt == "bitcrusher-on") {
			fxparams.bEnableBitcrusher = true;
			bFxParamChanged = true;
		}
		if (prompt == "bitcrusher-off") {
			fxparams.bEnableBitcrusher = false;
			bFxParamChanged = true;
		}
		if (prompt == "bitcrusher-bits") {
			std::cout << ">> enter bit depth (1-32)" << std::endl;
			std::cin >> prompt;
			auto depth = std::stoi(prompt);
			depth = clamp(depth, 0, 32);
			fxparams.bitCrusherNBits = depth;
			bFxParamChanged = true;
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
		if (prompt == "randompattern") {

			std::cout << "usage: randompattern <looptimes>. generated random pattern N times. play with 'pattern' command" << std::endl;
			NoteGenerator gen;
			std::cin >> prompt;
			int loopCount = std::stod(prompt);
			while (loopCount > 0) {
				auto temp = gen.randomPattern("synth1", 8, 33, 72);
				notes = temp;
				playPattern(temp, params.bpm);
				loopCount--;
			}
			std::cout << "generated random pattern, play with 'pattern' command" << std::endl;

		}
		if (prompt == "scale") {
			std::cout << "usage: scale <key pattern mode>. e.g. scale C# maj lydian" << std::endl;
			NoteGenerator gen;
			std::string keyStr, patStr, modeStr;
			std::cin >> keyStr >> patStr >> modeStr;
			// populate key, pattern, mode
			Key key = Scale::strToKey(keyStr);
			ScalePattern pattern = Scale::strToScalePattern(patStr);
			ScaleMode mode = Scale::strToScaleMode(modeStr);
			auto temp = gen.scalePattern(key, pattern, mode);
			playPattern(temp, params.bpm);
			std::cout << "now playing scale" << std::endl;
		}

		if (prompt == "loop") {
			std::cin >> prompt;
			int loopCount = std::stod(prompt);
			if (notes.size() == 0) {
				std::cout << "nothing to loop, use play to declare a sequence..." << std::endl;
			}
			else {
				while (loopCount > 0) {
					playPattern(notes, params.bpm);
					loopCount--;
				}
			}
		}

		if (prompt == "pattern") {
			if (notes.size() > 0) {
				playPattern(notes, params.bpm);
			}
			else {
				std::cout << "no notes to play!" << std::endl;
			}
		}
		if (prompt == "clear") {
			notes = {};
		}
		if (prompt == "mix") {
			std::cout << ">> mix <trackname> <dB (-60...0)>" << std::endl;
			std::vector<std::string> param;
			size_t paramCount = 0;
			std::string gainParams;
			while (paramCount < 2 && std::cin >> gainParams) {
				param.push_back(gainParams);
				paramCount++;
			}
			std::string track;
			track = param[0];
			float fGainDB = 0.f;
			std::sscanf(param[1].c_str(), "%f", &fGainDB);

			fGainDB = clamp(fGainDB, -60.f, 0.f);

			stream.updateTrackGainDB(track, fGainDB);

			bParamChanged = true;

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


void Prompt::playPattern(std::queue<NoteEvent> notes, size_t bpm)
{

	float now = 0.f;

	auto temp = sortTimeVal(notes);

	NoteEvent ev = static_cast<NoteEvent>(temp.front());

	while (!temp.empty()) {

		ev = static_cast<NoteEvent>(temp.front());
		// we are in 4/4
		float tv = ev.timeVal * 4 * 60.f * 1000.f / bpm;

		if (tv != now) {
			Sleep(tv - now);
		}

		now += tv - now;


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
					if (tv - now > 0) {
						Sleep(tv - now);
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


		if (tv - now < 0) {
			now = tv;
		}

	}

	// if "now" is before the end of a measure, sleep until the measure if over

	// find nearest multiple of "now" to the duration of one measure of 4/4 at the bpm:
	float m = 4.f * 60.f * 1000.f / bpm;
    // https://math.stackexchange.com/questions/291468/how-to-find-the-nearest-multiple-of-16-to-my-given-number-n
	float nearest = (m == 0) ? now : ceil(now / m) * m;
	float endOfMeasure = nearest - now;
	Sleep(endOfMeasure);

}