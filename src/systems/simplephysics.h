/*
    simplephysics.h: simple physics system
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
#ifndef _systems_simplephysics
#define _systems_simplephysics

#include "component.h"
#include "transform.h"
#include "collision.h"

struct SimplePhysicsObject {
    TransformSystem::IndexType transformId = TransformSystem::IndexType();
    CollisionSystem::IndexType colliderId = CollisionSystem::IndexType();
    bool collision = false;
    glm::vec2 velocity = glm::vec2(0.0f);
    glm::vec2 acceleration = glm::vec2(0.0f);
    glm::vec2 gravity = glm::vec2(0.0f);
};

class SimplePhysicsSystem {
public:
    using ComponentType = SimplePhysicsObject;
    using IndexType = SlotMapIndex;

    IndexType create(const TransformComponent& transform);
    IndexType create(const TransformComponent& transform, const CollisionComponent& collider);
    IndexType create(const TransformSystem::IndexType& transformId, CollisionSystem::IndexType& colliderId = CollisionSystem::IndexType(), bool collision = false);
    SimplePhysicsObject& get(const IndexType& i);
    void remove(const IndexType& i);

    void update(double dt);

    static std::shared_ptr<SimplePhysicsSystem> instance;

private:
    SlotMap<SimplePhysicsObject> objects;
};

using SimplePhysicsComponent = ComponentWrapper<SimplePhysicsSystem>;

#endif //_systems_simplephysics