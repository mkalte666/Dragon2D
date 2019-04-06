/*
    tsx.h: tiled map editor tileset format
    Copyright (C) 2019 Malte Kie�ling
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
#ifndef _util_tiled_tsx_h
#define _util_tiled_tsx_h

#include <string>
#include <vector>
#include <map>
#include "util/rect.h"

class Tsx {
public:
    Tsx();
    Tsx(const std::string& filename);

    int count = 0;
    int tilew = 0;
    int tileh = 0;
    int spacing = 0;
    int margin = 0;
    int tilecount = 0;
    int columns = 0;
    std::string name = "";
    std::string imageFilename ="";
    uint32_t firstgid = 0;
    std::multimap<int, FRect> colliders;
};

#endif //_util_tiled_tsx_h