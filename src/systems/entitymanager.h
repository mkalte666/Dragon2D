/*
    entitymanager.h: entity manager interface 
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
#ifndef _entitymanager_h
#define _entitymanger_h

#include "entity.h"
#include <vector>

class EntityManager {
public:
    void addEntity(EntityPtr& ptr);

    void removeEntity(EntityPtr& ptr);

    std::vector<EntityPtr> entites;
    static std::shared_ptr<EntityManager> instance;
};

#endif //_entitymanger_h