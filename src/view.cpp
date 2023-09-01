#pragma once

#include "view.h"
#include "viewbag.h"

#include "ImGuiFileDialog.h"

View::View()
	: m_window(
		SDL_CreateWindow(
			"S Y N T H C A S T L E",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			1280,
			720,
			(SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI)))
    , m_glContext(SDL_GL_CreateContext(m_window))
    , m_pEditorContext(ImNodes::EditorContextCreate())
    , m_cachedClickPos()
    , m_bSetPosOfLatestNode(false)
    , m_cachedViewBagSize(0)
{
    m_displays[NodeType::GAIN] = std::make_shared<GainDisplayCommand>();
    m_displays[NodeType::CONSTANT] = std::make_shared<ConstantDisplayCommand>();
    m_displays[NodeType::DELAY] = std::make_shared<DelayDisplayCommand>();
    m_displays[NodeType::ENVELOPE] = std::make_shared<EnvelopeDisplayCommand>();
    m_displays[NodeType::FILTER] = std::make_shared<FilterDisplayCommand>();
    m_displays[NodeType::OUTPUT] = std::make_shared<OutputDisplayCommand>();
    m_displays[NodeType::OSCILLATOR] = std::make_shared<OscillatorDisplayCommand>();
    m_displays[NodeType::QUAD_MIXER] = std::make_shared<MixerDisplayCommand>();
    m_displays[NodeType::SAMPLER] = std::make_shared<SamplerDisplayCommand>();
    m_displays[NodeType::SEQ] = std::make_shared<SeqDisplayCommand>();
    m_displays[NodeType::TRIG] = std::make_shared<TrigDisplayCommand>();
    const char* glsl_version = initSDL();
    SDL_GL_MakeCurrent(m_window, m_glContext);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto io = ImGui::GetIO();
    ImFont* pFont = io.Fonts->AddFontFromFileTTF("../Jost-Regular.ttf", 20.0f);
    ImNodes::CreateContext();
    ImGui::StyleColorsDark();
    ImNodes::StyleColorsDark();
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(m_window, m_glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);

    ImNodesIO& nodes_io = ImNodes::GetIO();
    nodes_io.LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;
    nodes_io.MultipleSelectModifier.Modifier = &ImGui::GetIO().KeyCtrl;

    ImNodesStyle& style = ImNodes::GetStyle();
    style.Flags |= ImNodesStyleFlags_GridLinesPrimary | ImNodesStyleFlags_GridSnapping;

}

void View::addListener(std::shared_ptr<ViewListener> listener) {
	m_listener = listener;
    for (auto const& [k,v] : m_displays) {
        v->addListener(listener);
    }
}

View::~View()
{

	ImNodes::EditorContextFree(m_pEditorContext);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImNodes::DestroyContext();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(m_glContext);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

const char* View::initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        printf("Error: %s\n", SDL_GetError());
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    return glsl_version;
}

void View::run()
{
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	bool done = false;

    while (!done) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                done = true;
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(m_window)) {
                done = true;
            }
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();

        ImGui::NewFrame();

        // Display the editor
        display();

        // Render the frame
        ImGui::Render();
        auto& io = ImGui::GetIO();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(
            clear_color.x * clear_color.w,
            clear_color.y * clear_color.w,
            clear_color.z * clear_color.w,
            clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(m_window);
    }
}

void View::display()
{
    ImNodes::EditorContextSet(m_pEditorContext);

    ImGuiStyle* style = &ImGui::GetStyle();
    style->Colors[ImGuiCol_Text] = ImVec4(0, 0, 0, 1.);
    style->Colors[ImGuiCol_WindowBg] = ImVec4(0., 0, 0, 1.);
    style->Colors[ImGuiCol_ChildBg] = ImVec4(0., 0, 0, 1.);
    style->Colors[ImGuiCol_TitleBg] = ImVec4(0., 0.1, 0, 1.);
    style->Colors[ImGuiCol_PopupBg] = ImVec4(0., 0, 0, 1.);
    style->Colors[ImGuiCol_FrameBg] = ImVec4(.32, 0.7, 0., 1.);
    style->Colors[ImGuiCol_FrameBgActive] = ImVec4(.32, 0.7, 0., 1.);
    style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(.32, 0.7, 0., 1.);

    ImNodesStyle& nodestyle = ImNodes::GetStyle();
    nodestyle.PinCircleRadius = 7;
    nodestyle.Colors[ImNodesCol_Pin] = IM_COL32(145, 145, 145, 200);
    nodestyle.Colors[ImNodesCol_PinHovered] = IM_COL32(229, 225, 0, 200);
    nodestyle.Colors[ImNodesCol_Link] = IM_COL32(227, 240, 182, 255);
    nodestyle.Colors[ImNodesCol_LinkHovered] = IM_COL32(228, 250, 60, 255);
    nodestyle.Colors[ImNodesCol_LinkSelected] = IM_COL32(225, 221, 0, 255);
    nodestyle.Colors[ImNodesCol_NodeBackground] = IM_COL32(217, 217, 217, 255);
    nodestyle.Colors[ImNodesCol_NodeBackgroundHovered] = IM_COL32(185, 196, 199, 255);
    nodestyle.Colors[ImNodesCol_NodeBackgroundSelected] = IM_COL32(185, 196, 199, 255);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0, 0, 0, 1));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, (ImVec4)ImColor(0, 0, 0, 1));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0, 0, 0, 1));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(227, 255, 99));
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 1));

    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 1));
    ImGui::Begin("S Y N T H C A S T L E", 0, ImGuiWindowFlags_NoTitleBar);
    ImGui::TextUnformatted("Keys: a=osc/c=const/e=envelope/f=filt/g=gain/m=mixer/o=output/t=trigger");

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
    ImNodes::BeginNodeEditor();

    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
        ImNodes::IsEditorHovered()) {
        bool bKeyReleased = false;
        if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_A)) {
            // something about getting the ID and setting the position
			m_listener->queueCreation("oscillator");
            bKeyReleased = true;
        }
        else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_V)) {
        }
        else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_C)) {
			m_listener->queueCreation("constant");
            bKeyReleased = true;
        }
        else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_D)) {
			m_listener->queueCreation("delay");
            bKeyReleased = true;
        }
        else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_T)) {
			m_listener->queueCreation("trig");
            bKeyReleased = true;
        }
        else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_E)) {
			m_listener->queueCreation("envelope");
            bKeyReleased = true;
        }
        else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_F)) {
			m_listener->queueCreation("filter");
            bKeyReleased = true;

        }
        else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_M)) {
            m_listener->queueCreation("mixer");
            bKeyReleased = true;
        }
        else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_G)) {
            m_listener->queueCreation("gain");
            bKeyReleased = true;
        }
        else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_X)) {
        }
        else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_O)) {
            m_listener->queueCreation("output");
            bKeyReleased = true;
        }
        else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_S)) {
            m_listener->queueCreation("seq");
            bKeyReleased = true;
        }
        else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_R)) {
            m_listener->queueCreation("sampler");
            bKeyReleased = true;
        }


        if (bKeyReleased) {
            m_cachedClickPos = ImGui::GetMousePosOnOpeningCurrentPopup();
            m_bSetPosOfLatestNode = true;
        }
    }

    const ViewBag viewbag = m_listener->snapshot();

    // display nodes
    for (auto const& [k, v] : viewbag.map) {
        if (m_displays.find(v.nodeType) != m_displays.end()) {
            m_displays[v.nodeType]->display(k, v);
        }
    }

    // display links
    for (auto const& [k, v] : viewbag.map) {
        auto snap = viewbag.map.at(k);
        auto id = k;
        for (auto const& [edgenum, edge] : snap.edges) {
            if (viewbag.map.find(edge.from) != viewbag.map.end()) {
                if (viewbag.map.at(edge.from).nodeType != NodeType::VALUE) {
                    continue;
                }
                ImNodes::Link(id, edge.from, edge.to);
            }
        }
	}

    ImNodes::EndNodeEditor();
    ImGui::PopStyleColor();

    m_listener->update();

    // set the latest created node to the last know mouse position
    if (m_bSetPosOfLatestNode && 
        (viewbag.map.size() > m_cachedViewBagSize)) {
        int min = 0;
		m_cachedViewBagSize = viewbag.map.size();
        for (const auto& [k, v] : viewbag.map) {
            if (k > min) {
                min = k;
            }
        }
        ImNodes::SetNodeScreenSpacePos(min, m_cachedClickPos);
        m_bSetPosOfLatestNode = false;
    }

    {
        int startAttr = 0;
        int endAttr = 0;
        bool bSnap = false;
        // CREATE LINKS BETWEEN NODES
        if (ImNodes::IsLinkCreated(&startAttr, &endAttr, &bSnap)) {
            if ((viewbag.map.find(startAttr) != viewbag.map.end()) &&
                (viewbag.map.find(endAttr) != viewbag.map.end())) {
                NodeType startType = viewbag.map.at(startAttr).nodeType;
                NodeType endType = viewbag.map.at(endAttr).nodeType;
                const bool bLinkValid = (startType != endType);
                if (bLinkValid) {
                    if (startType != NodeType::VALUE) {
                        std::swap(startAttr, endAttr);
                    }
                }
                m_listener->queueLink(startAttr, endAttr);
            }
        }
    }

    ImGui::End();
    ImGui::PopStyleColor(7);
}

void OutputDisplayCommand::display(int id, const NodeSnapshot& snapshot)
{

	ImGui::PushItemWidth(120.0f);
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(130,255,140, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(130,255,140, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(130,255,140, 255));
    ImNodes::BeginNode(id);
	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Output");
	ImNodes::EndNodeTitleBar();
    ImNodes::PopColorStyle();

    ImNodes::BeginInputAttribute(snapshot.params.at("left_id"));
	ImGui::TextUnformatted("Left Out");
	ImGui::ProgressBar(snapshot.params.at("display_left"), ImVec2(0.0f, 0.0f), " ");
	ImNodes::EndInputAttribute();

    ImNodes::BeginInputAttribute(snapshot.params.at("right_id"));
	ImGui::TextUnformatted("Right Out");
	ImGui::ProgressBar(snapshot.params.at("display_right"), ImVec2(0.0f, 0.0f), " ");
	ImNodes::EndInputAttribute();

	ImNodes::EndNode();
    ImGui::PopItemWidth();
}

void DelayDisplayCommand::display(int id, const NodeSnapshot& snapshot)
{

    ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(167, 219, 216, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(167, 219, 216, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(167, 219, 216, 255));

	ImGui::PushItemWidth(80.0f);

    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Delay");
    ImNodes::EndNodeTitleBar();

	ImNodes::BeginInputAttribute(snapshot.params.at("input_id"));
	ImGui::TextUnformatted("In");
	ImNodes::EndInputAttribute();

    auto d = snapshot.params.at("delay_ms");
    ImGui::DragFloat("Delay Time (ms)", &d, 0.5f, 0, 2999.);
    update(id, snapshot, "delay_ms", d);

    auto f = snapshot.params.at("feedback_ratio");
    ImGui::DragFloat("Feedback Ratio", &f, 0.01f, 0., 0.5);
    update(id, snapshot, "feedback_ratio", f);

    auto w = snapshot.params.at("drywet_ratio");
    ImGui::DragFloat("Dry/Wet Ratio", &w, 0.01f, 0., 1.);
    update(id, snapshot, "drywet_ratio", w);

    ImNodes::BeginOutputAttribute(id);
	const float text_width = ImGui::CalcTextSize("Out").x;
	ImGui::Indent(120.f + ImGui::CalcTextSize("Out").x - text_width);
	ImGui::TextUnformatted("Out");
	ImNodes::EndOutputAttribute();
	ImNodes::EndNode();
    ImGui::PopItemWidth();
    ImNodes::PopColorStyle();
}

void GainDisplayCommand::display(int id, const NodeSnapshot& snapshot)
{
	auto params = snapshot.params;

    ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(227, 105, 241, 255));
	ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(227, 105, 241, 255));
	ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(227, 105, 241, 255));

	ImNodes::BeginNode(id);
	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Gain");
	ImNodes::EndNodeTitleBar();

	ImNodes::BeginInputAttribute(params["input_id"]);
	ImGui::TextUnformatted("In");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(params["gainmod_id"]);
	ImGui::TextUnformatted("Mod");
	ImNodes::EndInputAttribute();

	ImGui::PushItemWidth(120.0f);
	auto g = snapshot.params.at("gain");
	ImGui::SliderFloat("Gain", &g, 0., 1.);
    update(id, snapshot, "gain", g);
	ImGui::PopItemWidth();

	ImNodes::BeginOutputAttribute(id);
	const float text_width = ImGui::CalcTextSize("Out").x;
	ImGui::Indent(120.f + ImGui::CalcTextSize("Out").x - text_width);
	ImGui::TextUnformatted("Out");
	ImNodes::EndOutputAttribute();
	ImNodes::EndNode();
    ImNodes::PopColorStyle();
}

void ConstantDisplayCommand::display(int id, const NodeSnapshot& snapshot)
{
	ImGui::PushItemWidth(120.0f);
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(0, 0, 0, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(0, 0, 0, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(0, 0, 0, 255));
	ImNodes::BeginNode(id);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Constant");
	ImNodes::EndNodeTitleBar();
    ImGui::PopStyleColor();
    ImNodes::PopColorStyle();

    auto v = snapshot.params.at("value");
	ImGui::DragFloat("Value", &v, 0.1f, 0, 1.);
    update(id, snapshot, "value", v);

	ImNodes::BeginOutputAttribute(id);
    const float text_width = ImGui::CalcTextSize("Out").x;
    ImGui::Indent(120.f + ImGui::CalcTextSize("Out").x - text_width);
	ImGui::TextUnformatted("Out");
	ImNodes::EndOutputAttribute();

	ImNodes::EndNode();
	ImGui::PopItemWidth();
}

void SeqDisplayCommand::display(int id, const NodeSnapshot& snapshot)
{
	ImGui::PushItemWidth(160.0f);
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(22,147,165, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(22,147,165, 230));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(22,147,165, 255));
	ImNodes::BeginNode(id);
	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("8-step Sequencer");
	ImNodes::EndNodeTitleBar();
    ImNodes::PopColorStyle();

	ImNodes::BeginInputAttribute(snapshot.params.at("trigin_id"));
	ImGui::TextUnformatted("Trig In");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(snapshot.params.at("reset_id"));
	ImGui::TextUnformatted("Reset");
	ImNodes::EndInputAttribute();



    auto step = snapshot.params.at("step");
    auto progress = ((1.+step) / 8.);

    // Typically we would use ImVec2(-1.0f,0.0f) or ImVec2(-FLT_MIN,0.0f) to use all available width,
    // or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
    ImGui::ProgressBar(progress, ImVec2(216, 0.0f), "");
    ImGui::NewLine();

    std::string str[8] = { "s1", "s2","s3","s4","s5","s6","s7","s8"};
    ImGui::PushID("set1");
    for (int i = 0; i < 8; i++) {
		ImGui::SameLine();
        auto val = snapshot.params.at(str[i]);
        ImGui::PushID(i);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(i / 7.0f, 0.5f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(i / 7.0f, 0.9f, 0.9f));
        ImGui::VSliderFloat("##v", ImVec2(22, 160), &val, 0.0f, 0.5f, "");
		update(id, snapshot, str[i], val);
        if (ImGui::IsItemActive() || ImGui::IsItemHovered())
            ImGui::SetTooltip("%.3f", val);
        ImGui::PopStyleColor(4);
        ImGui::PopID();
    }

    ImGui::NewLine();

    for (int i = 0; i < 8; i++) {
		ImGui::SameLine();
        auto val = snapshot.params.at(str[i]);
        ImGui::PushID(i);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(i / 7.0f, 0.5f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(i / 7.0f, 0.9f, 0.9f));
        auto bEnableStep = static_cast<bool>(snapshot.params.at("enable_" + str[i]));
        ImGui::Checkbox("##hidelabel", &bEnableStep);
		update(id, snapshot, "enable_" + str[i], bEnableStep);
        ImGui::PopStyleColor(4);
        ImGui::PopID();
    }

    auto g = (snapshot.params.at("gatemode") == 1.f) ? true : false;
	ImGui::Checkbox("Gate Mode", &g);
    update(id, snapshot, "gatemode", (float)g);

    ImGui::PopID();
	ImNodes::BeginOutputAttribute(id);
    const float text_width = ImGui::CalcTextSize("Out").x;
    ImGui::Indent(120.f + ImGui::CalcTextSize("Out").x - text_width);
	ImGui::TextUnformatted("Out");
	ImNodes::EndOutputAttribute();

	ImNodes::EndNode();
	ImGui::PopItemWidth();
}

void TrigDisplayCommand::display(int id, const NodeSnapshot& snapshot)
{
    ImGui::PushItemWidth(60.0f);
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(248,202,0, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(248,202,0, 230));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(248,202,0, 255));
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Trig");
    ImNodes::EndNodeTitleBar();

    int clicked = 0;
    if (ImGui::SmallButton("Trig!")) {
        clicked = 1;
    }
    update(id, snapshot, "trig", clicked);


    // first output is not a relay
    ImNodes::BeginOutputAttribute(id);
    std::string str = "Trig1";
	auto text_width = ImGui::CalcTextSize(str.c_str()).x;
	ImGui::Indent(60.f + ImGui::CalcTextSize(str.c_str()).x - text_width);
	ImGui::TextUnformatted(str.c_str());
	ImNodes::EndOutputAttribute();


    size_t numTrigs = static_cast<size_t>(snapshot.params.at("numtrigs"));
    for (size_t idx = 2; idx <= numTrigs; idx++) {
        std::string str = "Trig" + std::to_string(idx);
        std::string idstr = "trig" + std::to_string(idx);
        idstr += "_id";
        ImNodes::BeginOutputAttribute(snapshot.params.at(idstr));
		auto text_width = ImGui::CalcTextSize(str.c_str()).x;
		ImGui::Indent(60.f + ImGui::CalcTextSize(str.c_str()).x - text_width);
		ImGui::TextUnformatted(str.c_str());
		ImNodes::EndOutputAttribute();
    }

    auto v = snapshot.params.at("bpm");
    ImGui::DragFloat("BPM", &v, 0.5f, 0, 300.);
    update(id, snapshot, "bpm", v);
    // update progress animation speed
    float speed = v / 25;

    auto progress = snapshot.params.at("progress");
    auto progressDir = snapshot.params.at("progress_dir");
	progress += progressDir * speed * ImGui::GetIO().DeltaTime;

	if (progress >= +1.1f) { progress = +1.1f; progressDir *= -1.0f; }
	if (progress <= -0.1f) { progress = -0.1f; progressDir *= -1.0f; }

    update(id, snapshot, "progress", progress);
    update(id, snapshot, "progress_dir", progressDir);

    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(0,0,0));
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(72/255., 245/255., 66/255., progress));
    ImGui::ProgressBar(1.f, ImVec2(25.f, 0.0f), "");
    ImGui::PopStyleColor(2);

    clicked = 0;
    if (ImGui::SmallButton("Stop")) {
        clicked = 1;
		update(id, snapshot, "stop", clicked);
    }

    clicked = 0;
    if (ImGui::SmallButton("Start")) {
        clicked = 0;
		update(id, snapshot, "stop", clicked);
    }

    ImNodes::EndNode();
    ImGui::PopItemWidth();
    ImNodes::PopColorStyle();
}

void EnvelopeDisplayCommand::display(int id, const NodeSnapshot& snapshot)
{
	ImGui::PushItemWidth(120.0f);
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(163,49,189, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(163,49,189, 230));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(163,49,189, 255));
	ImNodes::BeginNode(id);
	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Envelope");
	ImNodes::EndNodeTitleBar();
    ImNodes::PopColorStyle();

	ImNodes::BeginInputAttribute(snapshot.params.at("input_id"));
	ImGui::TextUnformatted("In");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(snapshot.params.at("trig_id"));
	ImGui::TextUnformatted("Trig");
	ImNodes::EndInputAttribute();

    auto a = snapshot.params.at("attack_ms");
	ImGui::DragFloat("Attack Time (ms)", &a, 0.2f, 0., 1000.);
    update(id, snapshot, "attack_ms", a);

    auto d = snapshot.params.at("decay_ms");
	ImGui::DragFloat("Decay Time (ms)", &d, 0.2f, 0, 1000.);
    update(id, snapshot, "decay_ms", d);

    auto s = snapshot.params.at("sustain_db");
	ImGui::DragFloat("Sustain Level (dB)", &s, 1.f, -100, 0.);
    update(id, snapshot, "sustain_db", s);

    auto r = snapshot.params.at("release_ms");
    ImGui::DragFloat("Release Time (ms)", &r, 1.f, 0., 1000.);
    update(id, snapshot, "release_ms", r);

	ImNodes::BeginOutputAttribute(id);
    const float text_width = ImGui::CalcTextSize("Out").x;
    ImGui::Indent(120.f + ImGui::CalcTextSize("Out").x - text_width);
	ImGui::TextUnformatted("Out");
	ImNodes::EndOutputAttribute();

	ImNodes::EndNode();
	ImGui::PopItemWidth();
}



void OscillatorDisplayCommand::display(int id, const NodeSnapshot& snapshot)
{

	auto params = snapshot.params;
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(233,127,2, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(233,127,2, 230));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(233,127,2, 255));
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Oscillator");
    ImNodes::EndNodeTitleBar();
    ImNodes::PopColorStyle();

    ImNodes::BeginInputAttribute(params["freq_id"]);
    ImGui::PushItemWidth(120.0f);
    auto f = params["freq"];
    ImGui::DragFloat("Frequency", &f, 1.f, 0.00, 2000.);
    update(id, snapshot, "freq", f);
    ImGui::PopItemWidth();
    ImNodes::EndInputAttribute();

    ImNodes::BeginInputAttribute(params["modfreq_id"]);
    ImGui::TextUnformatted("FreqMod");
    ImNodes::EndInputAttribute();

    ImNodes::BeginInputAttribute(params["moddepth_id"]);
    ImGui::TextUnformatted("FreqModDepth");
    ImNodes::EndInputAttribute();

    ImGui::PushItemWidth(120.0f);
    auto t = params["tuning_coarse"];
    ImGui::DragFloat("Coarse Tuning", &t, 1.f, -36.00, 36.);
    update(id, snapshot, "tuning_coarse", t);
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(120.0f);
    auto tf = params["tuning_fine"];
    ImGui::DragFloat("Fine Tuning", &tf, 1.f, 0.00, 1.);
    update(id, snapshot, "tuning_fine", tf);
    ImGui::PopItemWidth();

    int selected = -1;

    // Simplified one-liner Combo() API, using values packed in a single constant string
    ImGui::PushItemWidth(120.0f);
    int w = (int)params["waveform"];
    ImGui::Combo("Waveform", &w, "Saw\0Sine\0Square\0Triangle\0S&H\0Noise\0");
    update(id, snapshot, "waveform", w);

    ImNodes::BeginOutputAttribute(id);
    const float text_width = ImGui::CalcTextSize("Out").x;
    ImGui::Indent(120.f + ImGui::CalcTextSize("Out").x - text_width);
    ImGui::TextUnformatted("Out");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
}

void MixerDisplayCommand::display(int id, const NodeSnapshot& snapshot)
{

	auto params = snapshot.params;
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(206, 171, 156, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(206, 171, 156, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(233,127,2, 230));

    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("QuadMix");
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(params["inputa_id"]);
    ImGui::TextUnformatted("A");
    ImNodes::EndInputAttribute();

    ImNodes::BeginInputAttribute(params["inputb_id"]);
    ImGui::TextUnformatted("B");
    ImNodes::EndInputAttribute();

    ImNodes::BeginInputAttribute(params["inputc_id"]);
    ImGui::TextUnformatted("C");
    ImNodes::EndInputAttribute();

    ImNodes::BeginInputAttribute(params["inputd_id"]);
    ImGui::TextUnformatted("D");
    ImNodes::EndInputAttribute();

    ImNodes::BeginOutputAttribute(id);
    ImGui::TextUnformatted("Mix");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
    ImNodes::PopColorStyle();
}

void SamplerDisplayCommand::display(int id, const NodeSnapshot& snapshot)
{
	auto params = snapshot.params;
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(235, 158, 168,255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(235, 158, 168,255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(235, 158, 168,255));

    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Sampler");
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(params["input_id"]);
    ImGui::TextUnformatted("Position");
    ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(params["startstop_id"]);
    ImGui::TextUnformatted("Start/Stop");
    ImNodes::EndInputAttribute();

    auto p = snapshot.stringParams.at("path");
    std::string filePathName = "";
    std::string dialName = "nodeid_" + std::to_string((int)(snapshot.params.at("node_id")));
    if (ImGui::Button("Browse...")) {
        ImGuiFileDialog::Instance()->OpenDialog(dialName, "Choose File", ".wav,.raw", ".");
    }
    if (ImGuiFileDialog::Instance()->Display(dialName, 0, ImVec2(300,300), ImVec2(1000,1000))) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
        }
        ImGuiFileDialog::Instance()->Close();
    }

    auto path = filePathName;
    ImGui::Text("Path: %s", p.c_str());
    if (path[0] == 'C') {
        update(id, snapshot, "path", path);
    }

    ImNodes::BeginOutputAttribute(id);
    ImGui::TextUnformatted("Out");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
    ImNodes::PopColorStyle();
}


void FilterDisplayCommand::display(int id, const NodeSnapshot& snapshot)
{
    auto params = snapshot.params;
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(189, 21, 80, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(189, 21, 80, 230));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(189, 21, 80, 255));
    ImNodes::BeginNode(id);
	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Lowpass Filter");
	ImNodes::EndNodeTitleBar();
    ImNodes::PopColorStyle();

    ImNodes::BeginInputAttribute(params["input_id"]);
	ImGui::TextUnformatted("In");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(params["freqmod_id"]);
	ImGui::TextUnformatted("Mod");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(params["moddepth_id"]);
	ImGui::TextUnformatted("Depth");
	ImNodes::EndInputAttribute();

	ImGui::PushItemWidth(120.0f);
    auto f = params["freq"];
	ImGui::DragFloat("Freq", &f, 0., 5000.);
    update(id, snapshot, "freq", f);
	ImGui::PopItemWidth();

	ImGui::PushItemWidth(120.0f);
    auto q = params["q"];
	ImGui::DragFloat("Q", &q, 0., 10.);
    update(id, snapshot, "q", q);
	ImGui::PopItemWidth();

	ImNodes::BeginOutputAttribute(id);
	const float text_width = ImGui::CalcTextSize("Out").x;
	ImGui::Indent(120.f + ImGui::CalcTextSize("Out").x - text_width);
	ImGui::TextUnformatted("Out");
	ImNodes::EndOutputAttribute();

	ImNodes::EndNode();
}

