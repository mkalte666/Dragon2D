/*
    entity.h: entity interface 
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
#ifndef _entity_h
#define _entity_h

#include <memory>
#include <python/python.h>

#include "component.h"

class Entity {
public:
    Entity();
    virtual ~Entity() = default;

    int id = -1;

    void addComponent(ComponentWrapperBasePtr component);

    void destroy();
    bool getIsDestroyed() const;

private:
    std::vector<ComponentWrapperBasePtr> components;
    bool isDestroyed;
};
using EntityPtr = std::shared_ptr<Entity>;

template <class EntityBase = Entity>
class PyEntityBase : public EntityBase {
public:
    using EntityBase::EntityBase;
};
#endif // _entity_h