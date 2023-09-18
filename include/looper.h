#include "node.h"
#include <fstream>
#include <ios>
#include <vector>
#include <cfloat>
#include "module.h"
#include "util.h"

#include "thirdparty/AudioFile.h"

class Looper : public Node 
{
public:
	Looper();
	virtual ~Looper() {}
	float process(float in) override;

	int lookupParam(std::string str) override {
		return m_lookup[str];
	}
	std::vector<std::string> paramStrings() override
	{
		std::vector<std::string> strings;
		for (std::unordered_map<std::string, int>::iterator iter = m_lookup.begin(); iter != m_lookup.end(); ++iter) {
			auto k = iter->first;
			strings.push_back(k);
		}

		return strings;
	}

	enum SamplerParams {
		NODE_ID,
		INPUT_ID,
		STOP,
		RECORD,
		ERASE,
		LOOP,
		LOOP_POINT_BEGIN,
		LOOP_POINT_END,
		REVERSE,
		SPEED,
		FILENAME,
		NUM_PARAMS
	};

private:
	// m_loop should be capped at 1 minute
	std::vector<float> m_loop; // m_loop is a mix of all the layers
	std::vector<float> m_buffer;
	// every time we layer, account for number of layers and mix
	// by .5
	size_t m_numLayers;
	size_t m_maxBufferSize; // governed by first layer in the loop
	std::unordered_map<std::string, int> m_lookup = {
		{"node_id", NODE_ID},
		{"stop", STOP},
		{"record", RECORD},
		{"erase", ERASE},
		{"loop", LOOP},
		{"loop_point_begin", LOOP_POINT_BEGIN},
		{"loop_point_end", LOOP_POINT_END},
		{"reverse", REVERSE},
		{"speed", SPEED},
		{"filename", FILENAME},
	};
	float m_sampleRate;
	float m_maxLoopTimeSec;
	AudioFile<float> audioFile;
	size_t m_startPos;
	double m_accum;
	double m_rate; // should map to speed until time-stretching
	// is implemented
	std::vector<float> m_samples;
	double m_increment;
	std::string m_path;
	bool m_bTriggered;
	size_t m_loopPointBegin;
	size_t m_loopPointEnd;
	bool m_bRecording; // we are recording
	bool m_bStop;
	bool m_bErase;
	bool m_bReverse;
};
