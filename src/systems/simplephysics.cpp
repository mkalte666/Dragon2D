/*
    simplephysics.cpp: simple physics system
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

#include "simplephysics.h"

std::shared_ptr<SimplePhysicsSystem> SimplePhysicsSystem::instance(nullptr);

SimplePhysicsSystem::IndexType SimplePhysicsSystem::create(const TransformComponent& transform)
{
    return create(transform.getIndex());
}

SimplePhysicsSystem::IndexType SimplePhysicsSystem::create(const TransformSystem::IndexType& transformId)
{
    SimplePhysicsObject obj;
    obj.transformId = transformId;
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
        // simple velocity 
        auto& transform = TransformSystem::instance->get(obj.transformId);
        auto actualAcceleration = obj.gravity + obj.acceleration;
        transform.position += obj.velocity * static_cast<float>(dt);
        //transform.position += actualAcceleration * static_cast<float>(dt * dt * 0.5);
        
        obj.velocity = obj.velocity + obj.acceleration * static_cast<float>(dt);
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
            .def_readwrite("gravity", &SimplePhysicsObject::gravity);
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
            .def("get", &SimplePhysicsComponent::get, py::return_value_policy::reference);
    }
};
PyType<SimplePhysicsComponent, PySimplePhysicsComponent, SimplePhysicsObject> pysimplephysicscomponent;

