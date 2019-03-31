/*
    entitymanager.cpp: entity manager implementation 
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
#include "entitymanager.h"

#include "python/python.h"

void EntityManager::addEntity(EntityPtr& ptr)
{
    entites.push_back(ptr);
    ptr->id = static_cast<int>(entites.size() - 1);

    ptr->initComponents();
}

void EntityManager::removeEntity(EntityPtr& ptr)
{
    if (ptr->id < 0 || ptr->id >= entites.size()) {
        return;
    }

    int oldId = ptr->id;
    ptr->id = -1;
    if (oldId == (entites.size() - 1)) {
        entites.pop_back();
        return;
    }

    entites[oldId].reset();
    entites[oldId] = *entites.rbegin();
    entites[oldId]->id = oldId;
    entites.pop_back();
}

std::shared_ptr<EntityManager> EntityManager::instance(nullptr);

class PyEntityManager : public EntityManager {
public:
    static void initModule(py::module& m)
    {
        py::class_<EntityManager, std::shared_ptr<EntityManager>> em(m, "EntityManager");
        em.def("addEntity", &EntityManager::addEntity);
        em.def("removeEntity", &EntityManager::removeEntity);
        m.attr("entityManager") = EntityManager::instance;
    }
};
PyType<PyEntityManager> entityManager;
