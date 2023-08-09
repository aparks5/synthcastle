#pragma once

#include "imnodes.h"
#include "node.h"
#include <imgui.h>
#include <SDL_scancode.h>
#include <algorithm>
#include <vector>
#include <string>

namespace ColorNodeEditorNS
{

struct Link
{
    int id;
    int start_attr, end_attr;
};

struct Editor
{
    ImNodesEditorContext* context = nullptr;
    std::vector<Node>     nodes;
    std::vector<Link>     links;
    int                   current_id = 0;
};


void ColorNodeEditorInitialize();
void ColorNodeEditorShow();
void ColorNodeEditorShutdown();

} // namespace example