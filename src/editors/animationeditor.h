/*
    animationeditor.h: animation editor 
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
#ifndef _editors_animationeditor_h
#define _editors_animationeditor_h

#include "editor.h"
#include "systems/sprite.h"
#include "systems/animation.h"

class AnimationEditor : public Editor {
public:
    AnimationEditor();
    virtual void update(double dt) override;
    virtual void menu() override;

private:
    bool showAnimationEditor = false;
    bool spriteSelected = false;
    bool spriteSelectorActive = false;
    SpriteSystem::IndexType spriteId = SpriteSystem::IndexType();
    std::string spriteFilename = "";
    bool animationSelected = false;
    bool animationSelectorActive = false;
    std::string animationFilename = "";
    AnimationSystem::IndexType animationId = SpriteSystem::IndexType();
};

#endif //_editors_animationeditor_h