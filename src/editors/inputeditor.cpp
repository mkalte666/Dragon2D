/*
    inputeditor.cpp: input editor 
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
#include "inputeditor.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include "systems/input.h"

InputEditor::InputEditor()
{
}

void InputEditor::update(double dt)
{
    if (!showInputEditor) {
        return;
    }
    ImGui::Begin("Input Editor", &showInputEditor);
    auto& inputs = InputSystem::instance->getLoadedInputs();
    for (auto&& input : inputs) {
        ImGui::PushID(&input);
        ImGui::BeginGroup();
        ImGui::InputText("Input Name", &input.second.name);
        ImGui::SameLine();
        std::string eventName = input.first;
        ImGui::Text("Event Name: %s", eventName.c_str());
        ImGui::Indent();
        ImGui::Checkbox("Inherit Paramater", &input.second.inherit);
        if (!input.second.inherit) {
            ImGui::SameLine();
            ImGui::InputInt("Parameter", &input.second.newParam);
            if (input.second.rawParam) {

            } else {
            }
        }
        ImGui::Unindent();
        ImGui::EndGroup();
        ImGui::Separator();
        ImGui::PopID();
    }
    ImGui::End();
}

void InputEditor::menu()
{
    if (ImGui::BeginMenu("Inputs")) {
        if (ImGui::MenuItem("Edit")) {
            showInputEditor = true;
        }
        ImGui::EndMenu();
    }
}
