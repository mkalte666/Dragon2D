/*
    sprite.h: sprite drawing system interface
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
#ifndef _systems_sprite_h
#define _systems_sprite_h

#include "systems/transform.h"
#include "util/rect.h"
#include <cstdint>

struct RawTextureData {
    uint32_t width = 0;
    uint32_t height = 0;
    void* hwData = nullptr;
};

struct Sprite {
    TransformSystem::IndexType transformId = TransformSystem::IndexType();
    glm::vec2 offset = glm::vec2(0);
    Rect source;
};

struct BatchSprite {
    Rect src;
    glm::vec2 pos;
    bool hFlip = false;
    bool vFlip = false;
};

struct SpriteBatch {
    TransformSystem::IndexType transformId = TransformSystem::IndexType();
    Rect boundary;
    std::vector<BatchSprite> batch;
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
    struct UniqueBatchIndex {
        SlotMapIndex texture;
        SlotMapIndex entry;
        uint8_t layer;
    };
    using IndexType = SlotMap<UniqueSpriteIndex>::IndexType;
    using BatchIndexType = SlotMap<UniqueBatchIndex>::IndexType;

    SpriteSystem();
    ~SpriteSystem();

    IndexType create(const TransformComponent& transformComponent, const std::string& filename, uint8_t layer);
    IndexType create(const TransformSystem::IndexType& transformId, const std::string& filename, uint8_t layer);
    Sprite& get(const IndexType& i);
    void remove(const IndexType& i);

    BatchIndexType createBatch(const TransformSystem::IndexType& transformId, const std::string& filename, uint8_t layer, const std::vector<BatchSprite>& inBatch);
    SpriteBatch& getBatch(const BatchIndexType& i);
    void removeBatch(const BatchIndexType& i);

    void update(double dt);

    const RawTextureData& getSpriteTextureData(IndexType i);
    const RawTextureData& getBatchTextureData(BatchIndexType i);

    static std::shared_ptr<SpriteSystem> instance;

private:
    std::unique_ptr<SpriteSystemData> data;
};

using SpriteComponent = ComponentWrapper<SpriteSystem>;

#endif //_systems_sprite_h