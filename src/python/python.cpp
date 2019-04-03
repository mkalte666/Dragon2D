/*
    python.cpp: python interface implementation
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

PyClassList* PyClassList::list = nullptr;

PyClassList::PyClassList()
    : next(nullptr)
{
    if (!list) {
        list = this;
    } else {
        list->append(this);
    }
}

void PyClassList::append(PyClassList* p)
{
    if (next == nullptr) {
        next = p;
    } else {
        next->append(p);
    }
}

void PyClassList::initAll(py::module &m)
{
    for (PyClassList* p = list; p != nullptr; p = p->next) {
        p->initModule(m);
    }
}

PYBIND11_EMBEDDED_MODULE(d2d, m)
{
    PyClassList::initAll(m);
}

void pyEval(const std::string& p)
{
    try {
        py::eval(p.c_str());
    } catch (std::exception e) {
        SDL_Log("Python During Eval of \n====\n%s\n====\nERROR: %s", p.c_str(), e.what());
    }
}
