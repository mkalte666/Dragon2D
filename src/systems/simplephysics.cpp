/*
    simplephysics.cpp: simple physics system
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

#include "simplephysics.h"
#include <glm/glm.hpp>

std::shared_ptr<SimplePhysicsSystem> SimplePhysicsSystem::instance(nullptr);

SimplePhysicsSystem::IndexType SimplePhysicsSystem::create(const TransformComponent& transform)
{
    return create(transform.getIndex());
}

SimplePhysicsSystem::IndexType SimplePhysicsSystem::create(const TransformComponent& transform, const CollisionComponent& collider)
{
    return create(transform.getIndex(), collider.getIndex(), true);
}

SimplePhysicsSystem::IndexType SimplePhysicsSystem::create(const TransformSystem::IndexType& transformId, CollisionSystem::IndexType& colliderId, bool collision)
{
    SimplePhysicsObject obj;
    obj.transformId = transformId;
    obj.colliderId = colliderId;
    obj.collision = collision;
    return objects.insert(obj);
}

SimplePhysicsObject& SimplePhysicsSystem::get(const IndexType& i)
{
    return objects[i];
}

void SimplePhysicsSystem::remove(const IndexType& i)
{
    objects.remove(i);
}

void SimplePhysicsSystem::update(double dt)
{
    for (auto&& obj : objects) {
        auto& transform = TransformSystem::instance->get(obj.transformId);
        glm::vec2 lastPosition = transform.position;
        glm::vec2 newPosition = lastPosition;
        glm::vec2 lastVelocity = obj.velocity;
        glm::vec2 newVelocity = lastVelocity;
        auto actualAcceleration = obj.gravity + obj.acceleration;
        bool wasColliding = !obj.collision || CollisionSystem::instance->checkCollision(obj.colliderId);

        newPosition += obj.velocity * static_cast<float>(dt);
        newVelocity = obj.velocity + actualAcceleration * static_cast<float>(dt);

        transform.position = newPosition;
        obj.velocity = newVelocity;

        if (!wasColliding) {
            // check for collision now
            bool colliding = CollisionSystem::instance->checkCollision(obj.colliderId);
            // :(
            if (colliding) {
                // find the rough point of collision
                glm::vec2 processedNewPosition = lastPosition;
                glm::vec2 dist = lastPosition - newPosition;
                // step back through the collision
                glm::vec2 postCollisionVelocity = glm::length(dist) * glm::normalize(lastVelocity);
                for (int steps = static_cast<int>(glm::length(dist)); colliding && steps > 0; --steps) {
                    postCollisionVelocity = glm::normalize(lastVelocity) * static_cast<float>(steps);
                    processedNewPosition = lastPosition + lastVelocity * static_cast<float>(dt);
                    transform.position = processedNewPosition;
                    // check if we are still colliding
                    colliding = CollisionSystem::instance->checkCollision(obj.colliderId);
                }
                // failsafe
                if (colliding) {
                    processedNewPosition = lastPosition;
                }
                // not colliding anymore
                if (glm::length(lastVelocity - postCollisionVelocity) > 0.1) {
                    // try single axis movement for x
                    postCollisionVelocity = lastVelocity - postCollisionVelocity;
                    float tmp = postCollisionVelocity.y;
                    postCollisionVelocity.y = 0.0f;
                    glm::vec2 xAxisPos = processedNewPosition + postCollisionVelocity * static_cast<float>(dt);
                    transform.position = xAxisPos;
                    colliding = CollisionSystem::instance->checkCollision(obj.colliderId);
                    if (colliding) {
                        // try y axis movement
                        postCollisionVelocity.y = tmp;
                        postCollisionVelocity.x = 0.0f;
                        glm::vec2 yAxisPos = processedNewPosition + postCollisionVelocity * static_cast<float>(dt);
                        transform.position = yAxisPos;
                        colliding = CollisionSystem::instance->checkCollision(obj.colliderId);
                        if (colliding) {
                            // rip any movememnt
                            postCollisionVelocity.x = 0.0f;
                        } else {
                            processedNewPosition = yAxisPos;
                        }
                    } else {
                        // x movement worked
                        processedNewPosition = xAxisPos;
                    }
                }

                transform.position = processedNewPosition;
                obj.velocity = postCollisionVelocity + obj.acceleration * static_cast<float>(dt);
            }
        }
        if (obj.maxVelocity.x > 0.0f && obj.maxVelocity.x < glm::abs(obj.velocity.x)) {
            obj.velocity.x = obj.maxVelocity.x * ((0.0f < obj.velocity.x) - (obj.velocity.x < 0.0f));
        }
        if (obj.maxVelocity.y > 0.0f && obj.maxVelocity.y < glm::abs(obj.velocity.y)) {
            obj.velocity.y = obj.maxVelocity.y * ((0.0f < obj.velocity.y) - (obj.velocity.y < 0.0f));
        }
    }
}

class PySimplePhysicsObject {
public:
    static void initModule(py::module& m)
    {
        py::class_<SimplePhysicsObject, std::shared_ptr<SimplePhysicsObject>> c(m, "SimplePhysicsObject");
        c
            .def_readwrite("velocity", &SimplePhysicsObject::velocity)
            .def_readwrite("acceleration", &SimplePhysicsObject::acceleration)
            .def_readwrite("gravity", &SimplePhysicsObject::gravity)
            .def_readwrite("maxVelocity", &SimplePhysicsObject::maxVelocity);
    }
};
PyType<SimplePhysicsObject, PySimplePhysicsObject, glm::vec2> pysimplephysicsobject;

class PySimplePhysicsComponent {
public:
    static void initModule(py::module& m)
    {
        py::class_<SimplePhysicsComponent, SimplePhysicsComponent::Ptr, ComponentWrapperBase> c(m, "SimplePhysicsComponent");
        c
            .def(py::init<const TransformComponent&>())
            .def(py::init<const TransformComponent&, const CollisionComponent&>())
            .def("get", &SimplePhysicsComponent::get, py::return_value_policy::reference);
    }
};
PyType<SimplePhysicsComponent, PySimplePhysicsComponent, SimplePhysicsObject, CollisionComponent> pysimplephysicscomponent;
