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

    TextureWrapper(TextureWrapper&& other)
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
        newSprite.source.srcSize.x = img->w;
        newSprite.source.srcSize.y = img->h;

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

inline void drawOne(
    const glm::vec2& camera,
    const TextureWrapper& texture,
    const Transform2D& transform,
    const glm::vec2& offset,
    const SourceSlice& source)
{

    SDL_Rect srcRect;
    srcRect.x = source.src.x;
    srcRect.y = source.src.y;
    srcRect.w = source.srcSize.x;
    srcRect.h = source.srcSize.y;

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
        transform.flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE || transform.flipVertical ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);
}

void SpriteSystem::update(double dt)
{
    // for each camera
    glm::vec2 cameraOffset = glm::vec2(0.0);

    // for each layer
    for (auto&& layer : data->layers) {
        // for each type of texture
        for (auto&& texture : layer) {
            auto& tex = data->textures[texture.first];
            // render single sprites
            for (auto&& sprite : texture.second->sprites) {
                const auto& transform = TransformSystem::instance->get(sprite.transformId);
                drawOne(cameraOffset, tex, transform, sprite.offset, sprite.source);
            }

            // render batches
            for (auto&& batch : texture.second->batches) {
                const auto& transform = TransformSystem::instance->get(batch.transformId);
                for (auto&& single : batch.batch) {
                    drawOne(cameraOffset, tex, transform, single.second, single.first);
                }
            }
        }
    }
}

class PySourceSlice {
public:
    static void initModule(py::module&m)
    {
        py::class_<SourceSlice> c(m, "SourceSlice");
        c
            .def(py::init<>())
            .def_readwrite("src", &SourceSlice::src)
            .def_readwrite("srcSize", &SourceSlice::srcSize);
    }
};

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
PyType<PySprite> pysprite;

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
PyType<PySpriteComponent> pyspritecomponent;