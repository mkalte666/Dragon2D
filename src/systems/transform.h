/*
    position.h: position system interface
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
#ifndef _position_h
#define _position_h

#include "util/slotmap.h"
#include <glm/vec2.hpp>

#include "systems/component.h"

struct Transform2D {
    Transform2D();
    Transform2D(glm::vec2 p);

    glm::vec2 position;
    glm::vec2 scale;
    double rotation;
    bool flipHorizontal;
    bool flipVertical;
};

class TransformSystem {
public:
    using ComponentType = Transform2D;
    using IndexType = SlotMap<Transform2D>::IndexType;

    template <class... Args>
    IndexType create(Args&&... args)
    {
        return positions.emplace(std::forward<Args>(args)...);
    }

    void remove(const IndexType& index);
    Transform2D& get(const IndexType& index);

    static std::shared_ptr<TransformSystem> instance;

private:
    SlotMap<Transform2D> positions;
};

using TransformComponent = ComponentWrapper<TransformSystem>;

#endif //_position_h