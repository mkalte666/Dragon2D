/*
    tsx.h: tiled map editor tileset format
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
#ifndef _util_tiled_tsx_h
#define _util_tiled_tsx_h

#include <string>
#include <vector>

class Tsx {
public:
    Tsx();
    Tsx(const std::string& filename);

    int count;
    int tilew;
    int tileh;
    int spacing;
    int margin;
    int tilecount;
    int columns;
    std::string name;
    std::string imageFilename;
    int firstgid;
};

#endif //_util_tiled_tsx_h