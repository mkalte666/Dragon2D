/*
    tsx.cpp: tiled map editor tileset format
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
#include "tsx.h"

#include <tinyxml2.h>

namespace xml = tinyxml2;

Tsx::Tsx()
{
}

Tsx::Tsx(const std::string& filename)
{
    xml::XMLDocument doc;
    auto err = doc.LoadFile(filename.c_str());
    if (err != xml::XMLError::XML_SUCCESS) {
        return;
    }

    auto tileset = doc.FirstChildElement("tileset");
    if (tileset == nullptr) {
        return;
    }

    name = tileset->Attribute("name");
    tilew = tileset->IntAttribute("tilewidth", 0);
    tileh = tileset->IntAttribute("tileheight", 0);
    spacing = tileset->IntAttribute("spacing", 0);
    margin = tileset->IntAttribute("margin", 0);
    count = tileset->IntAttribute("tilecount", 0);
    columns = tileset->IntAttribute("columns", 0);

    auto image = tileset->FirstChildElement("image");
    if (image == nullptr) {
        return;
    }

    // used for paths relative from the filename
    auto basepath = filename.substr(0, filename.find_last_of("/\\"));
    imageFilename = basepath + "/" + image->Attribute("source");

    // tile collision
    for (auto tileTag = tileset->FirstChildElement("tile"); tileTag; tileTag = tileTag->NextSiblingElement("tile")) {
        int tileId = tileTag->IntAttribute("id", -1);
        auto handleObject = [&](xml::XMLElement* objParentTag) {
            if (!objParentTag) {
                return;
            }
            for (auto objTag = objParentTag->FirstChildElement("object"); objTag; objTag = objTag->NextSiblingElement("object")) {
                FRect rect;
                rect.x = objTag->FloatAttribute("x", 0.0f);
                rect.y = objTag->FloatAttribute("y", 0.0f);
                rect.w = objTag->FloatAttribute("width", 0.0f);
                rect.h = objTag->FloatAttribute("height", 0.0f);
                colliders.insert(std::make_pair(tileId, rect));
            }
        };

        handleObject(tileTag->FirstChildElement("objectgroup"));
        handleObject(tileTag);
    }
}
