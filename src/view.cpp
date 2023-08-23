#pragma once

#include "view.h"
#include "viewbag.h"

View::View()
	: m_window(
		SDL_CreateWindow(
			"Dear ImGui SDL2+OpenGL3 example",
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
    m_displays[NodeType::OSCILLATOR] = std::make_shared<OscillatorDisplayCommand>();
    m_displays[NodeType::CONSTANT] = std::make_shared<ConstantDisplayCommand>();
    m_displays[NodeType::OUTPUT] = std::make_shared<OutputDisplayCommand>();
    const char* glsl_version = initSDL();
    SDL_GL_MakeCurrent(m_window, m_glContext);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto io = ImGui::GetIO();
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
    //signalFlowEditor.shutdown();
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
        //ImGui::ShowDemoWindow(nullptr);
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

    ImGui::Begin("signal_flow_editor");
    ImGui::TextUnformatted("Keys: a=osc/c=const/f=filt/g=gain/s=output/m=midi/x=mix/v=voice");

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
        else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_F)) {
        }
        else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_M)) {
        }
        else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_G)) {
            m_listener->queueCreation("gain");
            bKeyReleased = true;
        }
        else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_X)) {
        }
        else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_S)) {
            m_listener->queueCreation("output");
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
}

void OutputDisplayCommand::display(int id, const NodeSnapshot& snapshot)
{

	ImGui::PushItemWidth(120.0f);
    ImNodes::BeginNode(id);
	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Output");
	ImNodes::EndNodeTitleBar();

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

void GainDisplayCommand::display(int id, const NodeSnapshot& snapshot)
{
	auto params = snapshot.params;
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
}

void ConstantDisplayCommand::display(int id, const NodeSnapshot& snapshot)
{
	ImNodes::BeginNode(id);
	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Constant");
	ImNodes::EndNodeTitleBar();

	ImGui::PushItemWidth(120.0f);
    auto v = snapshot.params.at("value");
	ImGui::DragFloat("Value", &v, 0.5f, 0, 1.);
    update(id, snapshot, "value", v);
	ImGui::PopItemWidth();

	ImNodes::BeginOutputAttribute(id);
	ImGui::TextUnformatted("Out");
	ImNodes::EndOutputAttribute();

	ImNodes::EndNode();
}

void OscillatorDisplayCommand::display(int id, const NodeSnapshot& snapshot)
{

	auto params = snapshot.params;
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Oscillator");
    ImNodes::EndNodeTitleBar();

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

    ImNodes::BeginInputAttribute(params["modfreq_depth"]);
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
    ImGui::Combo("Waveform", &w, "Saw\0Sine\0Square\0Triangle\0S&H\0");
    update(id, snapshot, "waveform", w);

    ImNodes::BeginOutputAttribute(id);
    const float text_width = ImGui::CalcTextSize("Out").x;
    ImGui::Indent(120.f + ImGui::CalcTextSize("Out").x - text_width);
    ImGui::TextUnformatted("Out");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
}
