/*
    render.cpp: sprite drawing system implementation
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
#include "render.h"

#include "runtime/window.h"
#include "systems/camera.h"
#include "util/slotmap.h"
#include <GL/gl3w.h>
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

        // extract the texture id without crying too much about sdl internals
        SDL_GL_BindTexture(tex, nullptr, nullptr);
        GLuint textureId = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, reinterpret_cast<GLint*>(&textureId));
        rawData.hwData = reinterpret_cast<void*>(static_cast<ptrdiff_t>(textureId));
        rawData.width = static_cast<uint32_t>(surface->w);
        rawData.height = static_cast<uint32_t>(surface->h);
        SDL_GL_UnbindTexture(tex);

        SDL_FreeSurface(surface);
    }

    TextureWrapper(TextureWrapper&& other) noexcept
    {
        tex = other.tex;
        other.tex = nullptr;
        rawData = other.rawData;
    }

    TextureWrapper(const TextureWrapper& other) = delete;

    ~TextureWrapper()
    {
        if (tex) {
            SDL_DestroyTexture(tex);
        }

        tex = nullptr;
    }

    // for imgui and whatever
    const RawTextureData& getRawTextureData()
    {
        return rawData;
    }

    SDL_Texture* tex = nullptr;
    RawTextureData rawData;
    int refcount = 0;
};

class RenderSystemData {
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

    SlotMap<RenderSystem::UniqueSpriteIndex> lookup;
    SlotMap<RenderSystem::UniqueBatchIndex> lookupBatch;

    // filename->texture association
    std::map<std::string, TextureMap::IndexType> filenames;
};

std::shared_ptr<RenderSystem> RenderSystem::instance(nullptr);

RenderSystem::RenderSystem()
{
    data.reset(new RenderSystemData);
}

RenderSystem::~RenderSystem()
{
    data.reset();
}

RenderSystem::IndexType RenderSystem::createSprite(const TransformComponent& transformComponent, const std::string& filename, uint8_t layer)
{
    return createSprite(transformComponent.getIndex(), filename, layer);
}

RenderSystem::IndexType RenderSystem::createSprite(const TransformSystem::IndexType& transformId, const std::string& filename, uint8_t layer)
{
    Sprite newSprite;

    auto filenameIter = data->filenames.find(filename);
    if (filenameIter == data->filenames.end()) {
        auto img = IMG_Load(filename.c_str());
        if (!img) {
            SDL_Log("Cannot open file %s - %s", filename.c_str(), IMG_GetError());
            return RenderSystem::IndexType();
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
        textureLayer = drawLayer.insert(std::make_pair(texId, new RenderSystemData::TextureEntry())).first;
    }

    newSprite.transformId = transformId;

    auto spriteIndex = textureLayer->second->sprites.insert(std::move(newSprite));
    UniqueSpriteIndex index;
    index.texture = texId.toInt();
    index.entry = spriteIndex.toInt();
    index.layer = layer;

    return data->lookup.insert(index);
}

Sprite& RenderSystem::getSprite(const IndexType& i)
{
    auto& index = data->lookup[i];
    RenderSystemData::TextureMap::IndexType texId(index.texture);
    RenderSystemData::SpriteList::IndexType spriteId(index.entry);
    auto& layer = data->layers[index.layer];
    auto& spriteLayer = layer.find(texId);
    auto& sprite = spriteLayer->second->sprites.find(spriteId);

    return *sprite;
}

void RenderSystem::removeSprite(const IndexType& i)
{
    auto& index = data->lookup[i];
    RenderSystemData::TextureMap::IndexType texId(index.texture);
    RenderSystemData::SpriteList::IndexType spriteId(index.entry);
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

RenderSystem::BatchIndexType RenderSystem::createBatch(const TransformSystem::IndexType& transformId, const std::string& filename, uint8_t layer, const std::vector<BatchSprite>& inBatch)
{
    SpriteBatch newBatch;

    auto filenameIter = data->filenames.find(filename);
    if (filenameIter == data->filenames.end()) {
        auto img = IMG_Load(filename.c_str());
        if (!img) {
            SDL_Log("Cannot open file %s - %s", filename.c_str(), IMG_GetError());
            return RenderSystem::IndexType();
        }

        auto texId = data->textures.emplace(img);
        filenameIter = data->filenames.insert(std::make_pair(filename, texId)).first;
    }
    auto texId = filenameIter->second;

    data->textures[texId].refcount += 1;

    auto& drawLayer = data->layers[layer];
    auto textureLayer = drawLayer.find(texId);
    if (textureLayer == drawLayer.end()) {
        textureLayer = drawLayer.insert(std::make_pair(texId, new RenderSystemData::TextureEntry())).first;
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

SpriteBatch& RenderSystem::getBatch(const BatchIndexType& i)
{
    auto& index = data->lookupBatch[i];
    auto& layer = data->layers[index.layer];
    auto& spriteLayer = layer.find(index.texture);
    auto& batch = spriteLayer->second->batches.find(index.entry);

    return *batch;
}

void RenderSystem::removeBatch(const BatchIndexType& i)
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
    dstRect.x = roundf(transform.position.x + offset.x - camera.x);
    dstRect.y = roundf(transform.position.y + offset.y - camera.y);
    dstRect.w = roundf(transform.scale.x * static_cast<float>(srcRect.w));
    dstRect.h = roundf(transform.scale.y * static_cast<float>(srcRect.h));

    SDL_RenderCopyExF(
        Window::renderer,
        texture.tex,
        &srcRect,
        &dstRect,
        transform.rotation,
        nullptr,
        hFlip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE || vFlip ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);
}

void RenderSystem::update(double dt)
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
            cameraOffset -= glm::vec2(cameraWorldRect.size()) * 0.5f;
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

const RawTextureData& RenderSystem::getSpriteTextureData(IndexType i)
{
    static RawTextureData dummy;
    auto& index = data->lookup[i];
    auto textureIter = data->textures.find(index.texture);
    if (textureIter != data->textures.end()) {
        return textureIter->getRawTextureData();
    }
    return dummy;
}

const RawTextureData& RenderSystem::getBatchTextureData(BatchIndexType i)
{
    static RawTextureData dummy;
    auto& index = data->lookup[i];
    auto textureIter = data->textures.find(index.texture);
    if (textureIter != data->textures.end()) {
        return textureIter->getRawTextureData();
    }
    return dummy;
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

