/*
    init.h: systems init 
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
#include "systems/init.h"
#include "systems/entitymanager.h"
#include "systems/position.h"

void initSystems()
{
    EntityManager::instance = std::make_shared<EntityManager>();
    PositionSystem::instance = std::make_shared<PositionSystem>();
}

void finishSystems()
{
    PositionSystem::instance.reset();
    EntityManager::instance.reset();
}