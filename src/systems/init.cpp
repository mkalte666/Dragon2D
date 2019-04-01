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
#include "systems/sprite.h"
#include "systems/entitymanager.h"
#include "systems/transform.h"

void initSystems()
{

    TransformSystem::instance = std::make_shared<TransformSystem>();
    SpriteSystem::instance = std::make_shared<SpriteSystem>();

    // this thing can hold references to all the others, so destroy first and init last
    EntityManager::instance = std::make_shared<EntityManager>();
}

void finishSystems()
{
    // this thing can hold references to all the others, so destroy first and init last
    EntityManager::instance.reset();

    // now the rest
    SpriteSystem::instance.reset();
    TransformSystem::instance.reset();
}

void updateSystems(double dt)
{
    EntityManager::instance->update(dt);
    SpriteSystem::instance->update(dt);
}