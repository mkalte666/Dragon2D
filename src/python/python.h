/*
    python.h: python interface 
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
#ifndef _d2d_python_h
#define _d2d_python_h

#include <pybind11/embed.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <typeinfo>

namespace py = pybind11;

class PyClassList {
public:
    PyClassList();

    static void initAll(py::module& m);

protected:
    virtual void initModule(py::module& m) = 0;
    virtual std::string getName() const = 0;
    virtual std::vector<std::string> getDependencies() const = 0;

private:
    void append(PyClassList* p);
    PyClassList* next;
    static PyClassList* list;
};

template <class RealType, class WrapperType = typename RealType, class... Depends>
class PyType : public PyClassList {
protected:
    virtual std::string getName() const override
    {
        return std::string(typeid(RealType).name());
    }

    virtual std::vector<std::string> getDependencies() const override
    {
        std::vector<std::string> result = { typeid(Depends).name()... };

        return result;
    }

    virtual void initModule(py::module& m) override
    {
        WrapperType::initModule(m);
    }
};

void pyEval(const std::string& p);

class Python {
public:
    static pybind11::module runModule;
};

#endif //_d2d_python_h