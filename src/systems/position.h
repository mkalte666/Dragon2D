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

class PositionSystem {
public:
    using ComponentType = glm::vec2;
    using IndexType = SlotMap<glm::vec2>::IndexType;

    template<class ...Args>
    IndexType create(Args&&... args) {
        return positions.emplace(std::forward<Args>(args)...);
    }

    void remove(const IndexType& index);
    glm::vec2& get(const IndexType& index);

    static std::shared_ptr<PositionSystem> instance;

private:
    SlotMap<glm::vec2> positions;
};

using PositionComponent = ComponentWrapper<PositionSystem>;

#endif //_position_h