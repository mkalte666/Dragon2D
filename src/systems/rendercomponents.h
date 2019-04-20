/*
    rendercomponents.h: rendering components
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
#ifndef _systems_rendercomponents_h
#define _systems_rendercomponents_h

#include "systems/render.h"

class SpriteComponent : public ComponentWrapperBase {
public:
    using ComponentType = Sprite;
    using IndexType = SlotMapIndex;
    using Ptr = std::shared_ptr<SpriteComponent>;

    template <typename... Args>
    SpriteComponent(Args&&... args)
    {
        index = RenderSystem::instance->createSprite(std::forward<Args>(args)...);
        valid = true;
    }
    ~SpriteComponent();

    SpriteComponent(SpriteComponent&& other) = delete;
    SpriteComponent(const SpriteComponent& other) = delete;

    Sprite& get();

    IndexType getIndex() const;

private:
    IndexType index;
    bool valid;
};

class SpriteBatchComponent : public ComponentWrapperBase {
public:
    using ComponentType = SpriteBatch;
    using IndexType = SlotMapIndex;
    using Ptr = std::shared_ptr<SpriteComponent>;

    template <typename... Args>
    SpriteBatchComponent(Args&&... args)
    {
        index = RenderSystem::instance->createBatch(std::forward<Args>(args)...);
        valid = true;
    }
    ~SpriteBatchComponent();

    SpriteBatchComponent(SpriteComponent&& other) = delete;
    SpriteBatchComponent(const SpriteComponent& other) = delete;

    SpriteBatch& get();

    IndexType getIndex() const;

private:
    IndexType index;
    bool valid;
};

#endif //_systems_rendercomponents_h