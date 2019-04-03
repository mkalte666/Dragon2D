/*
    tilemap.h: Tilemap Drawing Interface (using the sprite batch system)
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
#ifndef _systems_tilemap_h
#define _systems_tilemap_h

#include "systems/sprite.h"

struct Tilemap {
    std::vector<SpriteSystem::BatchIndexType> batches;
};

class TilemapSystem {
public:
    using ComponentType = Tilemap;
    using IndexType = SlotMapIndex;

    IndexType create(const TransformComponent& transformComponent, const std::string& filename);
    IndexType create(const TransformSystem::IndexType& transformId, const std::string& filename);
    Tilemap& get(const IndexType& i);
    void remove(const IndexType& i);

    void update(double dt);

    static std::shared_ptr<TilemapSystem> instance;

private:
    SlotMap<Tilemap> tilemaps;
};

using TilemapComponent = ComponentWrapper<TilemapSystem>;

#endif //_systems_tilemap_h