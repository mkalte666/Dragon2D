/*
    sprite.h: sprite drawing system interface
    Copyright (C) 2019 Malte Kie�ling
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
#ifndef _systems_sprite_h
#define _systems_sprite_h

#include "systems/transform.h"
#include <cstdint>

struct SourceSlice {
    glm::ivec2 src = glm::ivec2(0);
    glm::ivec2 srcSize = glm::ivec2(0);
};

struct Sprite {
    TransformSystem::IndexType transformId;
    glm::vec2 offset = glm::vec2(0);
    SourceSlice source;
};

struct SpriteBatch {
    TransformSystem::IndexType transformId;
    std::vector<std::pair<SourceSlice, glm::vec2>> batch;
};

class SpriteSystemData;
class SpriteSystem {
public:
    using ComponentType = Sprite;
    struct UniqueSpriteIndex {
        uint64_t texture;
        uint64_t entry;
        uint8_t layer;
    };
    using IndexType = SlotMap<UniqueSpriteIndex>::IndexType;

    SpriteSystem();
    ~SpriteSystem();

    IndexType create(const TransformComponent& transformComponent, const std::string& filename, uint8_t layer);
    IndexType create(const TransformSystem::IndexType& transformId, const std::string& filename, uint8_t layer);
    Sprite& get(const IndexType& i);
    void remove(const IndexType& i);
    
    void update(double dt);

    static std::shared_ptr<SpriteSystem> instance;

private:
    std::unique_ptr<SpriteSystemData> data;
};

using SpriteComponent = ComponentWrapper<SpriteSystem>;

#endif //_systems_sprite_h