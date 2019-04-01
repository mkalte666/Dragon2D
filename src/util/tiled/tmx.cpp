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

#include <tinyxml2.h>
#include <algorithm>

namespace xml = tinyxml2;

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

    // fetch and load all tilesets
    for (auto tset = map->FirstChildElement("tileset"); tset; tset = tset->NextSiblingElement("tileset")) {
        auto filename = basepath + tset->Attribute("source");
        Tsx tileset(filename);
        tileset.firstgid = tset->IntAttribute("firstgid");
    }

    // sort them by gid
    std::sort(tilesets.begin(), tilesets.end(), [](const Tsx& l, const Tsx& r) {
        return l.firstgid < r.firstgid;
    });

    
}
