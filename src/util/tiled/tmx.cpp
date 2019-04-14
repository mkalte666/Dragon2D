/*
    tmx.cpp: tiled map editor tileset format
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
#include "tmx.h"

#include "util/xmlhelpers.h"
#include <algorithm>
#include <sstream>
#include <tinyxml2.h>

namespace xml = tinyxml2;

const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
const unsigned FLIPPED_VERTICALLY_FLAG = 0x40000000;
const unsigned FLIPPED_DIAGONALLY_FLAG = 0x20000000;

void extractProperties(Tmx::PropertyMap& target, xml::XMLElement* parent)
{
    auto propertiesTag = parent->FirstChildElement("properties");
    if (!propertiesTag) {
        return;
    }

    for (
        auto propertyTag = propertiesTag->FirstChildElement("property");
        propertyTag;
        propertyTag = propertyTag->NextSiblingElement("property")) {
        std::string name = nullAwareAttr(propertyTag->Attribute("name"));
        std::string value = nullAwareAttr(propertyTag->Attribute("value"));
        target[name] = value;
    }
}

Tmx::Tmx(const std::string& filename)
{
    // used for paths relative from the filename
    auto basepath = filename.substr(0, filename.find_last_of("/\\"));

    xml::XMLDocument doc;
    auto err = doc.LoadFile(filename.c_str());
    if (err != xml::XMLError::XML_SUCCESS) {
        return;
    }

    auto map = doc.FirstChildElement("map");
    if (!map) {
        return;
    }

    // load the properties
    extractProperties(properties, map);

    // fetch and load all tilesets
    for (auto tset = map->FirstChildElement("tileset"); tset; tset = tset->NextSiblingElement("tileset")) {
        auto filename = basepath + "/" + tset->Attribute("source");
        Tsx tileset(filename);
        tileset.firstgid = tset->UnsignedAttribute("firstgid", 0);
        tilesets.push_back(std::move(tileset));
    }

    // sort them by gid
    std::sort(tilesets.begin(), tilesets.end(), [](const Tsx& l, const Tsx& r) {
        return l.firstgid < r.firstgid;
    });

    for (auto layerTag = map->FirstChildElement("layer"); layerTag; layerTag = layerTag->NextSiblingElement("layer")) {
        // load the layer meta data
        Layer layer;
        layer.id = layerTag->IntAttribute("id", 0);
        layer.name = layerTag->Attribute("name");
        layer.x = layerTag->IntAttribute("x", 0);
        layer.y = layerTag->IntAttribute("y", 0);
        layer.width = layerTag->IntAttribute("width", 0);
        layer.height = layerTag->IntAttribute("height", 0);
        layer.opacity = layerTag->FloatAttribute("opacity", 1.0f);
        layer.visible = layerTag->BoolAttribute("visible", true);
        layer.offsetx = layerTag->IntAttribute("offsetx", 0);
        layer.offsety = layerTag->IntAttribute("offsety", 0);

        // properties first
        extractProperties(layer.properties, layerTag);

        auto dataTag = layerTag->FirstChildElement("data");
        if (!dataTag) {
            break;
        }

        for (auto chunkTag = dataTag->FirstChildElement("chunk"); chunkTag; chunkTag = chunkTag->NextSiblingElement("chunk")) {
            for (int i = 0; i < tilesets.size(); i++) {
                Chunk newChunk;
                newChunk.x = chunkTag->IntAttribute("x", 0);
                newChunk.y = chunkTag->IntAttribute("y", 0);
                newChunk.width = chunkTag->IntAttribute("width", 0);
                newChunk.height = chunkTag->IntAttribute("height", 0);

                layer.chunks[i].push_back(std::move(newChunk));
            }

            decode(layer, chunkTag->GetText());
        }

        layers.push_back(std::move(layer));
    }

    // go go go, objects next
    for (
        auto objLayerTag = map->FirstChildElement("objectgroup");
        objLayerTag;
        objLayerTag = objLayerTag->NextSiblingElement("objectgroup")) {
        // load properties
        ObjectLayer layer;
        extractProperties(layer.properties, objLayerTag);
        for (
            auto objTag = objLayerTag->FirstChildElement("object");
            objTag;
            objTag = objTag->NextSiblingElement("object")) {
            Object o;
            o.name = nullAwareAttr(objTag->Attribute("name"));
            o.type = nullAwareAttr(objTag->Attribute("type"));
            o.x = objTag->IntAttribute("x", 0);
            o.y = objTag->IntAttribute("y", 0);
            o.width = objTag->IntAttribute("width", 0);
            o.height = objTag->IntAttribute("height", 0);
            o.rotation = objTag->FloatAttribute("rotation", 0.0f);
            o.visible = objTag->BoolAttribute("visible", true);
            extractProperties(o.properties, objTag);

            layer.objects.push_back(o);
        }
        objectLayers.push_back(std::move(layer));
    }
}

void Tmx::decode(Layer& layer, const std::string& data)
{
    std::string token;
    std::istringstream ss(data);
    while (std::getline(ss, token, ',')) {
        uint32_t gid = static_cast<uint32_t>(std::stoll(token));
        bool flippedDiagonally = (gid & FLIPPED_DIAGONALLY_FLAG);
        bool flippedHorizontally = (gid & FLIPPED_HORIZONTALLY_FLAG) || flippedDiagonally;
        bool flippedVertically = (gid & FLIPPED_VERTICALLY_FLAG) || flippedDiagonally;

        gid &= ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);

        for (int i = 0; i < tilesets.size(); i++) {
            auto& tiles = layer.chunks[i].back().tiles;

            if (gid == 0 || gid < tilesets[i].firstgid || gid >= tilesets[i].firstgid + tilesets[i].count) {
                tiles.push_back(Tile());
                continue;
            }

            Tile t;
            t.hFlip = flippedHorizontally;
            t.vFlip = flippedVertically;
            t.id = gid - tilesets[i].firstgid;
            t.empty = false;
            tiles.push_back(t);
        }
    }
}
