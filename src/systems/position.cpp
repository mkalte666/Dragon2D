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

#include "position.h"
#include "python/python.h"

std::shared_ptr<PositionSystem> PositionSystem::instance(nullptr);

void PositionSystem::remove(const IndexType& index)
{
    positions.remove(index);
}

glm::vec2& PositionSystem::get(const IndexType& index)
{
    return positions[index];
}


class PyPositionComponent
{
public:
    static void initModule(py::module&m) {
        py::class_<PositionComponent, PositionComponent::Ptr, ComponentWrapperBase> c(m, "PositionComponent");
        c
            .def(py::init<>())
            .def(py::init<glm::vec2>())
            .def(py::init<float>())
            .def(py::init<float, float>())
            .def("get", &PositionComponent::get);
    }
};
PyType<PyPositionComponent> pycomponent;