/*
    init.h: systems init 
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
#include "systems/init.h"
#include "systems/animation.h"
#include "systems/camera.h"
#include "systems/collision.h"
#include "systems/entitymanager.h"
#include "systems/input.h"
#include "systems/simplephysics.h"
#include "systems/render.h"
#include "systems/tick.h"
#include "systems/tilemap.h"
#include "systems/transform.h"

void initSystems()
{

    TransformSystem::instance = std::make_shared<TransformSystem>();
    CameraSystem::instance = std::make_shared<CameraSystem>();
    RenderSystem::instance = std::make_shared<RenderSystem>();
    AnimationSystem::instance = std::make_shared<AnimationSystem>();
    TilemapSystem::instance = std::make_shared<TilemapSystem>();
    CollisionSystem::instance = std::make_shared<CollisionSystem>();
    SimplePhysicsSystem::instance = std::make_shared<SimplePhysicsSystem>();
    
    // this thing can hold references to all the others, so destroy first and init last
    EntityManager::instance = std::make_shared<EntityManager>();

    // and these ones might hold some entity callbacks
    InputSystem::instance = std::make_shared<InputSystem>();
    TickSystem::instance = std::make_shared<TickSystem>();
}

void finishSystemsEarly()
{
    // first to go because callbacks into python
    InputSystem::instance->earlyCleanup();
    TickSystem::instance->earlyCleanup();
}

void finishSystems()
{
    // this thing can hold references to all the others, so destroy first and init last
    EntityManager::instance.reset();

    // now the rest
    SimplePhysicsSystem::instance.reset();
    CollisionSystem::instance.reset();
    TilemapSystem::instance.reset();
    AnimationSystem::instance.reset();
    RenderSystem::instance.reset();
    CameraSystem::instance.reset();
    TransformSystem::instance.reset();

    // everything might have callbacks -> last
    // callbacks should (*prays*) not hold any references to anything else
    InputSystem::instance.reset();
    TickSystem::instance.reset();
}

void processEvent(const SDL_Event& event)
{
    InputSystem::instance->processEvent(event);
}

void updateSystems(double dt)
{
    InputSystem::instance->update(dt);
    CollisionSystem::instance->update(dt);
    EntityManager::instance->update(dt);
    SimplePhysicsSystem::instance->update(dt);
    TickSystem::instance->update(dt);
    AnimationSystem::instance->update(dt);
    RenderSystem::instance->update(dt);
    TilemapSystem::instance->update(dt);
}