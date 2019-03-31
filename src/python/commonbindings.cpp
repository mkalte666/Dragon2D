/*
    commonbindings.cpp: common bindings to python
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
#include "python.h"
#include <iostream>

#include <memory>

#include <glm/vec2.hpp>
#include <glm/glm.hpp>

#include <pybind11/operators.h>

// big and ugly test binding
class PyVec2 {
public:
    static void initModule(py::module& rootmod)
    {
        auto m = rootmod.def_submodule("glm", "some abstraction for some of the glm vector classes");
        py::class_<glm::vec2> c(m, "vec2");
        // constructors
        c
            .def(py::init<>())
            .def(py::init<float, float>())
            .def(py::init<float>());
        // members
        c
            .def_readwrite("x", &glm::vec2::x)
            .def_readwrite("y", &glm::vec2::y);

        // some operators
        c
            .def(py::self + py::self)
            .def(py::self - py::self)
            .def(py::self * py::self)
            .def(py::self *= float())
            .def(py::self * float());

        // geometric interesting
        m
            .def("length", &glm::length<2, float, glm::precision::highp>)
            .def("distance", &glm::distance<2, float, glm::precision::highp>)
            .def("dot", &glm::dot<2, float, glm::precision::highp>)
            .def("faceforward", &glm::faceforward<2, float, glm::precision::highp>)
            .def("normalize", &glm::normalize<2, float, glm::precision::highp>)
            .def("reflect", &glm::reflect<2, float, glm::precision::highp>)
            .def("refract", &glm::refract<2, float, glm::precision::highp>);
    }
};
PyType<PyVec2> pyvec2;
