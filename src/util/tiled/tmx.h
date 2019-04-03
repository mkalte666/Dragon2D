/*
    tsx.h: tiled map editor tilemap format
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
#ifndef _util_tiled_tmx_h
#define _util_tiled_tmx_h

#include "tsx.h"
#include <unordered_map>

class Tmx {
public:
    struct Tile {
        uint32_t id = 0;
        bool hFlip = false;
        bool vFlip = false;
    };
    struct Chunk {
        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;
        std::vector<Tile> tiles;
    };
    struct Layer {
        int id = 0; ///< the unique id of the layer.
        std::string name; ///< the name of the layer
        int x = 0; ///< layer x coordinate in tiles
        int y = 0; ///< layer y coordinate in tiles
        int width = 0; ///< layer width in tiles. Same as map size for fixed-size maps
        int height = 0; ///< layer height in tiles. Same as map size for fixed-size maps
        float opacity = 0.0f; ///< the opacity of the layer.
        bool visible = true; ///< if the layer is shown.
        int offsetx = 0; ///< x offset of the layer in pixels
        int offsety = 0; ///<i y offset of the layer in pixels
        std::unordered_map<int, std::vector<Chunk>> chunks; ///< chunks of the map, orderd by tileset id
    };
    Tmx(const std::string& filename);

    std::vector<Tsx> tilesets;
    std::vector<Layer> layers;

private:
    void decode(Layer& layer, const std::string& data);
};

#endif //_util_tiled_tmx_h