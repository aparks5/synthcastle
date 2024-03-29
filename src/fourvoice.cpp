#include "fourvoice.h"
#include "imnodes.h"

#define MAX_VOICES (4)

FourVoice::FourVoice(int voiceId)
	: Node(NodeType::FOUR_VOICE, "fourvoice", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
	, m_voiceId(voiceId)
	, m_counter(0)
	, m_cache(0)
{
}

void FourVoice::process() noexcept
{
	//if (params[ACTIVE] == 1) {
	//	//return m_cache;
	//}

	//m_cache = in;
	//return in;

}

void FourVoice::display()
{
	ImNodes::BeginNode(params[FourVoiceParams::NODE_ID]);
	ImNodes::BeginNodeTitleBar();
	ImGui::Text("Voice(%d)", m_voiceId);
	ImNodes::EndNodeTitleBar();

	ImNodes::BeginInputAttribute(params[FourVoiceParams::INPUT_ID]);
	ImGui::TextUnformatted("In");
	ImNodes::EndInputAttribute();

	ImNodes::BeginOutputAttribute(params[FourVoiceParams::NODE_ID]);
	ImGui::TextUnformatted("Voice Out");
	ImNodes::EndOutputAttribute();

	ImNodes::EndNode();

}



