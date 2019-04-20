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

#include "SDL.h"
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
        // layer might bring a z index
        if (auto iter = layer.properties.find("z"); iter != layer.properties.end()) {
            zLayerId = static_cast<uint8_t>(std::stoi(iter->second));
        }

        for (int tilesetId = 0; tilesetId < mapfile.tilesets.size(); tilesetId++) {
            auto& tileset = mapfile.tilesets[tilesetId];
            for (auto& chunk : layer.chunks[tilesetId]) {
                std::vector<BatchSprite> batch;
                batch.reserve(chunk.width * chunk.height);
                glm::vec2 chunkOffset(chunk.x * tileset.tilew, chunk.y * tileset.tileh);
                for (int chunkTileId = 0; chunkTileId < chunk.tiles.size(); chunkTileId++) {
                    auto& tile = chunk.tiles[chunkTileId];

                    if (tile.empty) {
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

                    // check for colliders in the tileset
                    auto colliderRange = tileset.colliders.equal_range(tile.id);
                    for (auto colliderIter = colliderRange.first; colliderIter != colliderRange.second; ++colliderIter) {
                        FRect rect = colliderIter->second;
                        // move rect to tile position
                        rect += sprite.pos;
                        // create collider
                        map.colliders.push_back(CollisionSystem::instance->create(transformId, rect));
                    }

                    batch.push_back(std::move(sprite));
                }
                auto batchId = RenderSystem::instance->createBatch(transformId, tileset.imageFilename, zLayerId, batch);
                // bounding rect of the chunk
                Rect boundary;
                boundary.w = chunk.width * tileset.tilew;
                boundary.h = chunk.height * tileset.tileh;
                boundary += glm::ivec2(chunkOffset);
                RenderSystem::instance->getBatch(batchId).boundary = boundary;
                map.batches.push_back(batchId);
            }
        }
    }

    // TODO: objects
    for (auto&& objLayer : mapfile.objectLayers) {
        glm::vec2 offset = glm::vec2(objLayer.offsetx, objLayer.offsety);
        for (auto&& obj : objLayer.objects) {
            try {
                if (obj.type.empty()) {
                    continue;
                }

                FRect rect(
                    static_cast<float>(obj.x),
                    static_cast<float>(obj.y),
                    static_cast<float>(obj.width),
                    static_cast<float>(obj.height));
                rect += offset;
                py::object classname = Python::runModule.attr(obj.type.c_str());
                classname(obj.name, rect);
                
            } catch (std::exception e) {
                SDL_Log("Error during creation of object %s - %s", obj.name.c_str(), e.what());
            }
        }
    }

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
            RenderSystem::instance->removeBatch(batch);
        }
        for (auto&& collider : iter->colliders) {
            CollisionSystem::instance->remove(collider);
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
