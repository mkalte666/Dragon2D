/*
    overlay.h: editor overlay
    Copyright (C) 2019 Malte Kieﬂling
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "overlay.h"

#include <SDL.h>
#include <imgui.h>

Overlay::Overlay()
    : visible(false)
{
}

void Overlay::update(double dt)
{
    // toggle visibility
    const Uint8* state = SDL_GetKeyboardState(NULL);
    auto&& modstate = SDL_GetModState();
    if (!visible) {
        if (modstate & KMOD_LCTRL && modstate & KMOD_LALT && state[SDL_SCANCODE_UP] && state[SDL_SCANCODE_DOWN]) {
            visible = true;
        }
    }

    if (!visible) {
        return;
    }

    ImGui::BeginMainMenuBar();
    for (auto&& editor : editors) {
        editor->menu();
    }
    if (ImGui::MenuItem("Hide Editor Overlay")) {
        visible = false;
    }
    ImGui::EndMainMenuBar();


    for (auto&& editor : editors) {
        editor->update(dt);
    }
}
