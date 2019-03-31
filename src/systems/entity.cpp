/*
    entity.h: entity implementation 
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
#include "entity.h"

#include "python/python.h"

#include <iostream>

Entity::Entity()
{

}

void Entity::initComponents()
{
}

class PyEntity : public PyEntityBase<> {
public:
    using PyEntityBase<>::PyEntityBase;
    static void initModule(py::module& m)
    {
        py::class_<Entity, PyEntityBase<>, EntityPtr> e(m, "Entity");
        e.def(py::init<>());
        e.def_readonly("id", &Entity::id);
    }
};
PyType<PyEntity> entity;
