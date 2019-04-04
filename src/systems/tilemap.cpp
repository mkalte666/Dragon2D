/*
    tilemap.cpp: Tilemap Drawing implementation (using the sprite batch system)
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
#include "tilemap.h"

#include "util/tiled/tmx.h"

std::shared_ptr<TilemapSystem> TilemapSystem::instance(nullptr);

TilemapSystem::IndexType TilemapSystem::create(const TransformComponent& transformComponent, const std::string& filename)
{
    return create(transformComponent.getIndex(), filename);
}

TilemapSystem::IndexType TilemapSystem::create(const TransformSystem::IndexType& transformId, const std::string& filename)
{
    Tilemap map;

    Tmx mapfile(filename);
    for (int layerId = 0; layerId < mapfile.layers.size(); layerId++) {
        uint8_t zLayerId = static_cast<uint8_t>(layerId);
        auto& layer = mapfile.layers[layerId];
        for (int tilesetId = 0; tilesetId < mapfile.tilesets.size(); tilesetId++) {
            auto& tileset = mapfile.tilesets[tilesetId];
            for (auto& chunk : layer.chunks[tilesetId]) {
                std::vector<BatchSprite> batch;
                batch.reserve(chunk.width * chunk.height);
                glm::vec2 chunkOffset(chunk.x * tileset.tilew, chunk.y * tileset.tileh);
                for (int chunkTileId = 0; chunkTileId < chunk.tiles.size(); chunkTileId++) {
                    auto& tile = chunk.tiles[chunkTileId];

                    if (tile.id == 0) {
                        continue;
                    }
                    BatchSprite sprite;
                    sprite.src.x = tileset.margin + (tile.id % tileset.columns) * (tileset.tilew + tileset.margin * 2 + tileset.spacing);
                    sprite.src.y = tileset.margin + (tile.id / tileset.columns) * (tileset.tileh + tileset.margin * 2 + tileset.spacing);
                    sprite.src.w = tileset.tilew;
                    sprite.src.h = tileset.tileh;

                    sprite.pos.x = static_cast<float>((chunkTileId % chunk.width) * tileset.tilew);
                    sprite.pos.y = static_cast<float>((chunkTileId / chunk.width) * tileset.tileh);
                    sprite.pos += chunkOffset;

                    sprite.hFlip = tile.hFlip;
                    sprite.vFlip = tile.vFlip;

                    batch.push_back(std::move(sprite));
                }
                map.batches.push_back(SpriteSystem::instance->createBatch(transformId, tileset.imageFilename, zLayerId, batch));
            }
        }
    }

    // TODO: objects

    // instance the class object (if provided) and the eval tag
    if (auto iter = mapfile.properties.find("instance"); iter != mapfile.properties.end()) {
        if (!iter->second.empty()) {
            pyEval(iter->second);
        }
    }
    if (auto iter = mapfile.properties.find("eval"); iter != mapfile.properties.end()) {
        if (!iter->second.empty()) {
            pyEval(iter->second);
        }
    }

    return tilemaps.insert(map);
}

Tilemap& TilemapSystem::get(const IndexType& i)
{
    return tilemaps[i];
}

void TilemapSystem::remove(const IndexType& i)
{
    if (auto iter = tilemaps.find(i); iter != tilemaps.end()) {
        for (auto&& batch : iter->batches) {
            SpriteSystem::instance->removeBatch(batch);
        }
    }
    tilemaps.remove(i);
}

void TilemapSystem::update(double dt)
{
}

class PyTilemap {
public:
    static void initModule(py::module& m)
    {
        py::class_<Tilemap> c(m, "Tilemap");
        c
            .def(py::init<>());
    }
};
PyType<Tilemap, PyTilemap> pytilemap;

class PyTilemapComponent {
public:
    static void initModule(py::module& m)
    {
        py::class_<TilemapComponent, TilemapComponent::Ptr, ComponentWrapperBase> c(m, "TilemapComponent");
        c
            .def(py::init<const TransformComponent&, const std::string&>())
            .def("get", &TilemapComponent::get, py::return_value_policy::reference);
    }
};
PyType<TilemapComponent, PyTilemapComponent, ComponentWrapperBase, Tilemap> pytilemapcomponent;