/*
    sprite.h: sprite drawing system implementation
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
#include "sprite.h"

#include "runtime/window.h"
#include "systems/camera.h"
#include "util/slotmap.h"
#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>

class TextureWrapper {
public:
    TextureWrapper(SDL_Surface* surface)
    {
        if (!surface) {
            return;
        }

        tex = SDL_CreateTextureFromSurface(Window::renderer, surface);
        if (!tex) {
            SDL_Log("Cannot create Texture Wrapper %s", SDL_GetError());
        }

        SDL_FreeSurface(surface);
    }

    TextureWrapper(TextureWrapper&& other) noexcept
    {
        tex = other.tex;
        other.tex = nullptr;
    }

    TextureWrapper(const TextureWrapper& other) = delete;

    ~TextureWrapper()
    {
        if (tex) {
            SDL_DestroyTexture(tex);
        }

        tex = nullptr;
    }

    SDL_Texture* tex = nullptr;
    int refcount = 0;
};

class SpriteSystemData {
public:
    // the textures
    using TextureMap = SlotMap<TextureWrapper>;
    TextureMap textures;

    // sprites and sprite batches
    using SpriteList = SlotMap<Sprite>;
    using SpriteBatchList = SlotMap<SpriteBatch>;
    struct TextureEntry {
        SpriteList sprites;
        SpriteBatchList batches;
    };

    // a layer, sorted per texture
    using SpriteLayer = std::unordered_map<TextureMap::IndexType, std::unique_ptr<TextureEntry>>;
    // all the layers
    using Layers = std::array<SpriteLayer, 255>;
    Layers layers;

    SlotMap<SpriteSystem::UniqueSpriteIndex> lookup;
    SlotMap<SpriteSystem::UniqueBatchIndex> lookupBatch;

    // filename->texture association
    std::map<std::string, TextureMap::IndexType> filenames;
};

std::shared_ptr<SpriteSystem> SpriteSystem::instance(nullptr);

SpriteSystem::SpriteSystem()
{
    data.reset(new SpriteSystemData);
}

SpriteSystem::~SpriteSystem()
{
    data.reset();
}

SpriteSystem::IndexType SpriteSystem::create(const TransformComponent& transformComponent, const std::string& filename, uint8_t layer)
{
    return create(transformComponent.getIndex(), filename, layer);
}

SpriteSystem::IndexType SpriteSystem::create(const TransformSystem::IndexType& transformId, const std::string& filename, uint8_t layer)
{
    Sprite newSprite;

    auto filenameIter = data->filenames.find(filename);
    if (filenameIter == data->filenames.end()) {
        auto img = IMG_Load(filename.c_str());
        if (!img) {
            SDL_Log("Cannot open file %s - %s", filename.c_str(), IMG_GetError());
            return SpriteSystem::IndexType();
        }
        // FIXME: does this really need to default? probably yes.
        newSprite.source.w = img->w;
        newSprite.source.h = img->h;

        auto texId = data->textures.emplace(img);
        filenameIter = data->filenames.insert(std::make_pair(filename, texId)).first;
    }
    auto texId = filenameIter->second;

    data->textures[texId].refcount += 1;

    auto& drawLayer = data->layers[layer];
    auto textureLayer = drawLayer.find(texId);
    if (textureLayer == drawLayer.end()) {
        textureLayer = drawLayer.insert(std::make_pair(texId, new SpriteSystemData::TextureEntry())).first;
    }

    newSprite.transformId = transformId;

    auto spriteIndex = textureLayer->second->sprites.insert(std::move(newSprite));
    UniqueSpriteIndex index;
    index.texture = texId.toInt();
    index.entry = spriteIndex.toInt();
    index.layer = layer;

    return data->lookup.insert(index);
}

Sprite& SpriteSystem::get(const IndexType& i)
{
    auto& index = data->lookup[i];
    SpriteSystemData::TextureMap::IndexType texId(index.texture);
    SpriteSystemData::SpriteList::IndexType spriteId(index.entry);
    auto& layer = data->layers[index.layer];
    auto& spriteLayer = layer.find(texId);
    auto& sprite = spriteLayer->second->sprites.find(spriteId);

    return *sprite;
}

void SpriteSystem::remove(const IndexType& i)
{
    auto& index = data->lookup[i];
    SpriteSystemData::TextureMap::IndexType texId(index.texture);
    SpriteSystemData::SpriteList::IndexType spriteId(index.entry);
    auto& layer = data->layers[index.layer];
    auto& textures = data->textures;
    auto spriteLayerIter = layer.find(texId);
    auto textureIter = textures.find(texId);
    // is that even a texture in the layer
    if (spriteLayerIter != layer.end()) {
        auto& sprites = spriteLayerIter->second->sprites;
        // that sprite is still alive my friend
        if (sprites.find(spriteId) != sprites.end()) {
            sprites.remove(spriteId);
            // make sure the texture still actually exsists
            if (textureIter != textures.end()) {
                textureIter->refcount--;
                if (!textureIter->refcount) {
                    textures.remove(texId);
                }
            }
        }
    }
}

SpriteSystem::BatchIndexType SpriteSystem::createBatch(const TransformSystem::IndexType& transformId, const std::string& filename, uint8_t layer, const std::vector<BatchSprite>& inBatch)
{
    SpriteBatch newBatch;

    auto filenameIter = data->filenames.find(filename);
    if (filenameIter == data->filenames.end()) {
        auto img = IMG_Load(filename.c_str());
        if (!img) {
            SDL_Log("Cannot open file %s - %s", filename.c_str(), IMG_GetError());
            return SpriteSystem::IndexType();
        }

        auto texId = data->textures.emplace(img);
        filenameIter = data->filenames.insert(std::make_pair(filename, texId)).first;
    }
    auto texId = filenameIter->second;

    data->textures[texId].refcount += 1;

    auto& drawLayer = data->layers[layer];
    auto textureLayer = drawLayer.find(texId);
    if (textureLayer == drawLayer.end()) {
        textureLayer = drawLayer.insert(std::make_pair(texId, new SpriteSystemData::TextureEntry())).first;
    }

    newBatch.transformId = transformId;
    newBatch.batch = inBatch;

    auto batchIndex = textureLayer->second->batches.insert(std::move(newBatch));
    UniqueBatchIndex index;
    index.texture = texId.toInt();
    index.entry = batchIndex.toInt();
    index.layer = layer;

    return data->lookupBatch.insert(index);
}

SpriteBatch& SpriteSystem::getBatch(const BatchIndexType& i)
{
    auto& index = data->lookupBatch[i];
    auto& layer = data->layers[index.layer];
    auto& spriteLayer = layer.find(index.texture);
    auto& batch = spriteLayer->second->batches.find(index.entry);

    return *batch;
}

void SpriteSystem::removeBatch(const BatchIndexType& i)
{
    auto& index = data->lookupBatch[i];
    auto& layer = data->layers[index.layer];
    auto& textures = data->textures;
    auto spriteLayerIter = layer.find(index.texture);
    auto textureIter = textures.find(index.texture);
    // is that even a texture in the layer
    if (spriteLayerIter != layer.end()) {
        auto& batches = spriteLayerIter->second->batches;
        // that sprite is still alive my friend
        if (batches.find(index.entry) != batches.end()) {
            batches.remove(index.entry);
            // make sure the texture still actually exsists
            if (textureIter != textures.end()) {
                textureIter->refcount--;
                if (!textureIter->refcount) {
                    textures.remove(index.texture);
                }
            }
        }
    }
}

inline void drawOne(
    const glm::vec2& camera,
    const TextureWrapper& texture,
    const Transform2D& transform,
    const glm::vec2& offset,
    const Rect& source,
    bool hFlip,
    bool vFlip)
{

    SDL_Rect srcRect;
    srcRect.x = source.x;
    srcRect.y = source.y;
    srcRect.w = source.w;
    srcRect.h = source.h;

    SDL_FRect dstRect;
    dstRect.x = transform.position.x + offset.x - camera.x;
    dstRect.y = transform.position.y + offset.y - camera.y;
    dstRect.w = transform.scale.x * static_cast<float>(srcRect.w);
    dstRect.h = transform.scale.y * static_cast<float>(srcRect.h);

    SDL_RenderCopyExF(
        Window::renderer,
        texture.tex,
        &srcRect,
        &dstRect,
        transform.rotation,
        nullptr,
        hFlip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE || vFlip ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);
}

void SpriteSystem::update(double dt)
{
    SDL_Rect fullViewport;
    SDL_RenderGetViewport(Window::renderer, &fullViewport);
    // for each camera
    for (auto&& camera : CameraSystem::instance->getCameras()) {
        SDL_Rect viewport;
        if (!camera.fillTarget) {
            viewport = { camera.viewport.x, camera.viewport.y, camera.viewport.w, camera.viewport.h };
        } else {
            viewport = fullViewport;
        }
        SDL_RenderSetViewport(Window::renderer, &viewport);
        glm::vec2 cameraOffset = TransformSystem::instance->get(camera.transformId).position + camera.offset;
        Rect cameraWorldRect(glm::ivec2(cameraOffset), glm::ivec2(viewport.w, viewport.h));

        // centerd camera?
        if (camera.ceneterd) {
            cameraOffset -= glm::vec2(cameraWorldRect.size())*0.5f;
            cameraWorldRect -= glm::ivec2(glm::vec2(cameraWorldRect.size()) * 0.5f);
        }

        // for each layer
        for (auto&& layer : data->layers) {
            // for each type of texture
            for (auto&& texture : layer) {
                auto& tex = data->textures[texture.first];
                // render single sprites
                for (auto&& sprite : texture.second->sprites) {
                    const auto& transform = TransformSystem::instance->get(sprite.transformId);
                    drawOne(cameraOffset, tex, transform, sprite.offset, sprite.source, transform.flipHorizontal, transform.flipVertical);
                }

                // render batches
                for (auto&& batch : texture.second->batches) {
                    const auto& transform = TransformSystem::instance->get(batch.transformId);
                    // naive view frustim culling
                    if (batch.boundary.w > 0 || batch.boundary.h > 0) {
                        auto brect = batch.boundary;
                        brect += glm::ivec2(transform.position);
                        if (!cameraWorldRect.intersect(brect)) {
                            continue;
                        }
                    }
                    for (auto&& single : batch.batch) {
                        drawOne(cameraOffset, tex, transform, single.pos, single.src, single.hFlip, single.vFlip);
                    }
                }
            }
        }

        if (!camera.fillTarget) {
        }
    }

    SDL_RenderSetViewport(Window::renderer, &fullViewport);
    glm::vec2 cameraOffset = glm::vec2(0.0);
}

class PySprite {
public:
    static void initModule(py::module& m)
    {
        py::class_<Sprite> c(m, "Sprite");
        c
            .def(py::init<>())
            .def_readwrite("offset", &Sprite::offset)
            .def_readwrite("source", &Sprite::source);
    }
};
PyType<Sprite, PySprite, glm::vec2> pysprite;

class PySpriteComponent {
public:
    static void initModule(py::module& m)
    {
        py::class_<SpriteComponent, SpriteComponent::Ptr, ComponentWrapperBase> c(m, "SpriteComponent");
        c
            .def(py::init<const TransformComponent&, const std::string&, uint8_t>())
            .def("get", &SpriteComponent::get, py::return_value_policy::reference);
    }
};
PyType<SpriteComponent, PySpriteComponent, ComponentWrapperBase, Sprite> pyspritecomponent;