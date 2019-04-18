/*
    animation.h: animation system
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
#ifndef _systems_animation_h
#define _systems_animation_h

#include "systems/component.h"
#include "systems/sprite.h"
#include "util/rect.h"

struct Frame {
    double duration = 0.0f;
    Rect src = Rect();
};

struct Animation {
    std::string name = "";
    std::vector<Frame> frames;
    size_t currentFrame = 0;
    double frameTime = 0.0;
    bool loop = true;
    bool playing = false;
};

struct AnimatedSprite {
    SpriteSystem::IndexType spriteId;
    std::vector<Animation> animations;
    size_t current = 0;
    std::string defaultAnimation = "";
    void play(const std::string& name);
    void pause();
    void resume();
    void stop();

    void load(const std::string& filename);
    void save(const std::string& filename);
};

class AnimationSystem {
public:
    using ComponentType = AnimatedSprite;
    using IndexType = SlotMapIndex;

    IndexType create(const SpriteSystem::IndexType& spriteId, const std::string& filename);
    IndexType create(const SpriteComponent& sprite, const std::string& filename);
    AnimatedSprite& get(const IndexType& i);
    void remove(const IndexType& i);

    void update(double dt);

    static std::shared_ptr<AnimationSystem> instance;

private:
    SlotMap<AnimatedSprite> animations;
};

#endif // _systems_animation_h