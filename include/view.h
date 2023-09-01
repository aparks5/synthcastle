#pragma once

#include <memory>
#include "events.h"

#include "viewlistener.h"
#include "nodetypes.h"

#include "imnodes.h"
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL2/SDL_opengles2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif

class NodeDisplayCommand
{
public:
	NodeDisplayCommand() {}
	virtual ~NodeDisplayCommand() {}
	virtual void display(int id, const NodeSnapshot& snapshot) = 0;
	void addListener(std::shared_ptr<ViewListener> listener)
	{
		m_listener = listener;
	}
protected:
	std::shared_ptr<ViewListener> m_listener;
	void update(int id, const NodeSnapshot& snap, std::string param, float newval) {
		if (newval != snap.params.at(param)) {
			m_listener->queueUpdate(id, param, newval);
		}
	}

	void update(int id, const NodeSnapshot& snap, std::string param, std::string str) {
		if (str != snap.stringParams.at(param)) {
			m_listener->queueUpdate(id, param, str);
		}
	}

};

class View
{
public:
	View();
	virtual ~View();
	void run();
	void addListener(std::shared_ptr<ViewListener> listener);
private:
	void display();
	const char* initSDL();
	SDL_Window* m_window;
	SDL_GLContext m_glContext;
	ImNodesEditorContext* m_pEditorContext;
	std::shared_ptr<ViewListener> m_listener;
	std::unordered_map<NodeType, std::shared_ptr<NodeDisplayCommand>> m_displays;
	ViewBag m_bag;

	ImVec2 m_cachedClickPos;
	bool m_bSetPosOfLatestNode;
	size_t m_cachedViewBagSize;
	float m_progress;
	float m_progressDir;

};

class GainDisplayCommand : public NodeDisplayCommand
{
public:
	GainDisplayCommand() {}
	void display(int id, const NodeSnapshot& snapshot) override;
};

class DelayDisplayCommand : public NodeDisplayCommand
{
public:
	DelayDisplayCommand() {}
	void display(int id, const NodeSnapshot& snapshot) override;
};

class ConstantDisplayCommand : public NodeDisplayCommand
{
public:
	ConstantDisplayCommand() {}
	void display(int id, const NodeSnapshot& snapshot) override;
};

class MixerDisplayCommand : public NodeDisplayCommand
{
public:
	MixerDisplayCommand() {}
	void display(int id, const NodeSnapshot& snapshot) override;
};

class SamplerDisplayCommand : public NodeDisplayCommand
{
public:
	SamplerDisplayCommand() {}
	void display(int id, const NodeSnapshot& snapshot) override;
};

class TrigDisplayCommand : public NodeDisplayCommand
{
public:
	TrigDisplayCommand()
	{}
	void display(int id, const NodeSnapshot& snapshot) override;
};

class OscillatorDisplayCommand : public NodeDisplayCommand
{
public:
	OscillatorDisplayCommand() {}
	void display(int id, const NodeSnapshot& snapshot) override;
};

class SeqDisplayCommand : public NodeDisplayCommand
{
public:
	SeqDisplayCommand() {}
	void display(int id, const NodeSnapshot& snapshot) override;
};

class OutputDisplayCommand : public NodeDisplayCommand
{
public:
	OutputDisplayCommand() {}
	void display(int id, const NodeSnapshot& snapshot) override;
};

class FilterDisplayCommand : public NodeDisplayCommand
{
public:
	FilterDisplayCommand() {}
	void display(int id, const NodeSnapshot& snapshot) override;
};

class EnvelopeDisplayCommand : public NodeDisplayCommand
{
public:
	EnvelopeDisplayCommand() {}
	void display(int id, const NodeSnapshot& snapshot) override;
};









