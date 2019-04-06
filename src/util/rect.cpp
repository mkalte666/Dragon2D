/*
    rect.h: rect implementation 
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

#include "python/python.h"
#include "rect.h"

class PyRect {
public:
    static void initModule(py::module& m)
    {
        py::class_<Rect> c(m, "Rect");
        c
            .def(py::init<>())
            .def_readwrite("x", &Rect::x)
            .def_readwrite("y", &Rect::y)
            .def_readwrite("w", &Rect::w)
            .def_readwrite("h", &Rect::h);
    }
};
PyType<Rect, PyRect> pyrect;

class PyFRect {
public:
    static void initModule(py::module& m)
    {
        py::class_<FRect> c(m, "FRect");
        c
            .def(py::init<>())
            .def_readwrite("x", &FRect::x)
            .def_readwrite("y", &FRect::y)
            .def_readwrite("w", &FRect::w)
            .def_readwrite("h", &FRect::h);
    }
};
PyType<FRect, PyFRect> pyfrect;

class PyDRect {
public:
    static void initModule(py::module& m)
    {
        py::class_<DRect> c(m, "DRect");
        c
            .def(py::init<>())
            .def_readwrite("x", &DRect::x)
            .def_readwrite("y", &DRect::y)
            .def_readwrite("w", &DRect::w)
            .def_readwrite("h", &DRect::h);
    }
};
PyType<DRect, PyDRect> pyrdect;