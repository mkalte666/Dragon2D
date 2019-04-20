/*
    animationeditor.cpp: animation editor 
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
#include "animationeditor.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include "filedialog.h"

#include "runtime/filename.h"

AnimationEditor::AnimationEditor()
{
    name = "Animation Editor";
    spriteFilename = Filename::gameFile("");
}

void AnimationEditor::update(double dt)
{
    if (!showAnimationEditor) {
        return;
    }

    // animation editor
    ImGui::Begin("Animation Editor", &showAnimationEditor);
    if (ImGui::Button("Select Animation") && spriteSelected) {
        animationSelectorActive = true;
        if (animationSelected) {
            animationSelected = false;
            AnimationSystem::instance->remove(animationId);
        }
        if (animationFilename.empty()) {
            animationFilename = Filename::gameFile("/");
        }
    }

    if (animationSelectorActive && spriteSelected) {
        animationSelected = FileDialog("Select Animation", animationFilename, animationSelectorActive);
        if (animationSelected) {
            animationSelectorActive = false;
            animationId = AnimationSystem::instance->create(spriteId, animationFilename);
            // try saving once for the sake of file creation
            AnimationSystem::instance->get(animationId).save(animationFilename);
        }
    }

    if (animationSelected) {
        auto& animation = AnimationSystem::instance->get(animationId);
        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            animation.save(animationFilename);
        }
        ImGui::SameLine();
        if (ImGui::Button("Reload")) {
            animation.load(animationFilename);
        }
        ImGui::Separator();

        for (auto&& subAnimation : animation.animations) {
            ImGui::PushID(&subAnimation);
            if (ImGui::TreeNode("namelabel", "Name: %s", subAnimation.name.c_str())) {
                if (ImGui::Button("Play")) {
                    animation.play(subAnimation.name);
                }
                ImGui::InputText("Name", &subAnimation.name);
                ImGui::Checkbox("loop", &subAnimation.loop);
                // by index for swapping
                bool frameListHasChanged = false;
                for (size_t i = 0; i < subAnimation.frames.size() && !frameListHasChanged; i++) {
                    auto& frame = subAnimation.frames[i];
                    ImGui::PushID(static_cast<int>(i));
                    if (ImGui::TreeNode("framelabel", "Frame Id: %d", i)) {
                        ImGui::InputDouble("Duration", &frame.duration, 0.01, 0.1);
                        int posArr[2] = { frame.src.x, frame.src.y };
                        ImGui::InputInt2("Position", posArr);
                        frame.src.x = posArr[0];
                        frame.src.y = posArr[1];

                        int sizArr[2] = { frame.src.w, frame.src.h };
                        ImGui::InputInt2("Size", sizArr);
                        frame.src.w = sizArr[0];
                        frame.src.h = sizArr[1];

                        bool hasOrderButton = false;
                        if (i > 0) {
                            if (ImGui::Button("Move Up")) {
                                std::swap(subAnimation.frames[i], subAnimation.frames[i - 1]);
                                frameListHasChanged = true;
                            }
                            hasOrderButton = true;
                        }
                        if (i < subAnimation.frames.size() - 1) {
                            if (hasOrderButton) {
                                ImGui::SameLine();
                            }
                            if (ImGui::Button("Move Down")) {
                                std::swap(subAnimation.frames[i], subAnimation.frames[i + 1]);
                                frameListHasChanged = true;
                            }
                            hasOrderButton = true;
                        }

                        if (hasOrderButton) {
                            ImGui::SameLine();
                        }
                        if (ImGui::Button("Duplicate Below")) {
                            subAnimation.frames.insert(subAnimation.frames.begin() + i, Frame(frame));
                            frameListHasChanged = true;
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Delete")) {
                            frameListHasChanged = true;
                            subAnimation.frames.erase(subAnimation.frames.begin() + i);
                        }
                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                } // frame loop

                // add button
                if (ImGui::Button("Add Frame")) {
                    subAnimation.frames.push_back(Frame());
                }
                ImGui::TreePop();
            }
            ImGui::PopID();
        } // loop

        // add button
        if (ImGui::Button("Add new")) {
            animation.animations.push_back(Animation());
        }
    } // animation selected

    ImGui::End();

    //===================================
    // sprite (animation) preview drawing
    ImGui::Begin("Sprite Preview");
    if (ImGui::Button("Select Sprite") && !spriteSelectorActive) {
        spriteSelectorActive = true;
        if (spriteSelected) {
            spriteSelected = false;
            RenderSystem::instance->removeSprite(spriteId);
        }
        if (spriteFilename.empty()) {
            spriteFilename = Filename::gameFile("/");
        }
    }

    if (spriteSelectorActive) {
        spriteSelected = FileDialog("Select Sprite", spriteFilename, spriteSelectorActive);
        if (spriteSelected) {
            spriteSelectorActive = false;
            spriteId = RenderSystem::instance->createSprite(SlotMapIndex(), spriteFilename, 0);
        }
    }

    if (spriteSelected) {
        ImGui::Separator();
        auto& sprite = RenderSystem::instance->getSprite(spriteId);
        const auto& hwData = RenderSystem::instance->getSpriteTextureData(spriteId);
        ImVec2 imageSize(static_cast<float>(sprite.source.w), static_cast<float>(sprite.source.h));
        ImVec2 uv0;
        uv0.x = static_cast<float>(sprite.source.x) / static_cast<float>(hwData.width);
        uv0.y = static_cast<float>(sprite.source.y) / static_cast<float>(hwData.height);
        ImVec2 uv1;
        uv1.x = static_cast<float>(sprite.source.x + sprite.source.w) / static_cast<float>(hwData.width);
        uv1.y = static_cast<float>(sprite.source.y + sprite.source.h) / static_cast<float>(hwData.height);
        ImGui::Image(hwData.hwData,
            imageSize,
            uv0,
            uv1);
    }
    ImGui::End();
}

void AnimationEditor::menu()
{
    if (ImGui::BeginMenu("Animation")) {
        if (ImGui::MenuItem("Editor")) {
            showAnimationEditor = true;
        }

        ImGui::EndMenu();
    }
}