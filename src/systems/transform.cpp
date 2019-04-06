/*
    position.h: position system implementation
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

#include "transform.h"
#include "python/python.h"

Transform2D::Transform2D()
    : position(0.0)
    , scale(1.0)
    , rotation(0)
    , flipHorizontal(false)
    , flipVertical(false)
{
}

Transform2D::Transform2D(glm::vec2 p)
    : position(p)
    , scale(1.0)
    , rotation(0)
    , flipHorizontal(false)
    , flipVertical(false)
{
}

class PyTransform2D {
public:
    static void initModule(py::module& m)
    {
        py::class_<Transform2D> c(m, "Transform2D");
        c
            .def(py::init<>())
            .def(py::init<glm::vec2>())
            .def_readwrite("position", &Transform2D::position)
            .def_readwrite("scale", &Transform2D::scale)
            .def_readwrite("rotation", &Transform2D::rotation)
            .def_readwrite("flipHorizontal", &Transform2D::flipHorizontal)
            .def_readwrite("flipVertical", &Transform2D::flipVertical);
    }
};
PyType<Transform2D, PyTransform2D, glm::vec2> pytransform2d;

std::shared_ptr<TransformSystem> TransformSystem::instance(nullptr);

void TransformSystem::remove(const IndexType& index)
{
    positions.remove(index);
}

Transform2D& TransformSystem::get(const IndexType& index)
{
    if (auto iter = positions.find(index); iter != positions.end()) {
        return *iter;
    }

    defaultTransform = Transform2D();
    return defaultTransform;
}

class PyTransformComponent {
public:
    static void initModule(py::module& m)
    {
        py::class_<TransformComponent, TransformComponent::Ptr, ComponentWrapperBase> c(m, "TransformComponent");
        c
            .def(py::init<>())
            .def(py::init<glm::vec2>())
            .def("get", &TransformComponent::get, py::return_value_policy::reference);
    }
};
PyType<TransformComponent, PyTransformComponent, ComponentWrapperBase, Transform2D> pytransformcomponent;

