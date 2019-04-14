/*
    filedialog.cpp: Implementation of the file dialog for imgui
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

#include "filedialog.h"

#include <filesystem>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#ifdef WIN32
#include <Windows.h>
#endif

#if __cplusplus > 201703L
namespace fs = std::filesystem;
#else
namespace fs = std::experimental::filesystem;
#endif

bool FileDialog(const std::string& name, std::string& result, bool& visible)
{
    fs::path path = fs::system_complete(fs::path(result));
    bool bResult = false;

    ImGui::Begin(name.c_str(), &visible);

    // filename editor
    std::string filename = "";
    if (!fs::is_directory(path)) {
        filename = path.filename().string();
    }
    if (ImGui::InputText("##filename", &filename)) {
        if (fs::is_directory(path)) {
            path.append(filename);
        } else {
            path = path.parent_path().append(filename);
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Ok", ImVec2(100, 0))) {
        bResult = true;
    }
    ImGui::Separator();

    // drive picker
#ifdef WIN32
    auto current = path.begin()->string();
    ImGui::PushItemWidth(50);
    if (ImGui::BeginCombo("##Drive", current.c_str())) {
        auto driveMask = GetLogicalDrives();
        for (int i = 0; i < 'Z' - 'A'; i++) {
            int mask = 1 << i;
            if (!(driveMask & mask)) {
                continue;
            }

            std::string drive;
            drive += (char)('A' + i);
            bool isSelected = current == drive;
            if (ImGui::Selectable(drive.c_str(), isSelected)) {
                path = fs::path(drive + ":/");
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();
#endif

    // breadcrumbs
    fs::path fromHere;
    float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    ImGuiStyle& style = ImGui::GetStyle();
    for (auto& component : path) {
        fromHere.append(component);
        float last_button_x2 = ImGui::GetItemRectMax().x;
        float next_button_x2 = last_button_x2 + style.ItemSpacing.x + ImGui::GetItemRectSize().x;
        if (next_button_x2 < window_visible_x2) {
            ImGui::SameLine();
        }
        if (ImGui::Button(component.string().c_str())) {
            path = fromHere;
            break;
        }
    }

    ImGui::Separator();

    // files and directories
    ImGui::BeginChild("file picker");
    fs::path dir = path;
    if (!fs::is_directory(dir)) {
        dir = dir.parent_path();
    }
    for (auto& file : fs::directory_iterator(dir)) {
        auto filePath = file.path();
        auto filename = filePath.filename().string();

        if (fs::is_directory(filePath)) {
            filename += "/";
            if (ImGui::Selectable(filename.c_str())) {
                path = filePath;
            }
        } else {
            bool selected = false;
            if (ImGui::Selectable(filename.c_str(), &selected, ImGuiSelectableFlags_AllowDoubleClick)) {
                path = filePath;
                if (ImGui::IsMouseDoubleClicked(0)) {
                    bResult = true;
                }
            }
        }
    }
    ImGui::EndChild();

    // done
    ImGui::End();

    result = path.string();

    return bResult;
}
