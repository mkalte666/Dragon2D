/*
    collision.h: collision system
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
#ifndef _systems_collision_h
#define _systems_collision_h

#include "component.h"
#include "transform.h"
#include "util/rect.h"

struct Collider {
    TransformSystem::IndexType transformId = TransformSystem::IndexType();
    FRect aabb = FRect();
};

struct CollisionSystemData;

class CollisionSystem {
public:
    using ComponentType = Collider;
    using IndexType = SlotMapIndex;
    
    CollisionSystem();
    ~CollisionSystem();
    IndexType create(const TransformComponent& transform, const FRect& aabb);
    IndexType create(const TransformSystem::IndexType& transformId, const FRect& aabb);
    Collider& get(const IndexType& i);
    void remove(const IndexType& i);

    void update(double dt);
    bool checkCollision(const IndexType& i);

    static std::shared_ptr<CollisionSystem> instance;

private:
    std::unique_ptr<CollisionSystemData> data;
};

using CollisionComponent = ComponentWrapper<CollisionSystem>;

#endif // _systems_collision_h