/*
    tick.cpp: tick system
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
#include "tick.h"

#include "SDL.h"

std::shared_ptr<TickSystem> TickSystem::instance(nullptr);

TickSystem::TickSystem()
{
}

TickSystem::IndexType TickSystem::create(TickCallback callback)
{
    return tickCallbacks.insert(callback);
}

TickCallback& TickSystem::get(const IndexType& i)
{
    static TickCallback dumbNothing = [](double) {};

    if (auto iter = tickCallbacks.find(i); iter != tickCallbacks.end()) {
        return *iter;
    }

    return dumbNothing;
}

void TickSystem::remove(const IndexType& i)
{
    tickCallbacks.remove(i);
}

void TickSystem::update(double dt)
{
    for (auto&& callback : tickCallbacks) {
        try {
            if (callback) {
                callback(dt);
            }
        } catch (std::exception e) {
            SDL_Log("Error while trying to tick %s", e.what());
        }
    }
}

void TickSystem::earlyCleanup()
{
    tickCallbacks.clear();
}

class PyTickComponent {
public:
    static void initModule(py::module& m)
    {
        py::class_<TickComponent, TickComponent::Ptr, ComponentWrapperBase> c(m, "TickComponent");
        c
            .def(py::init<TickCallback>())
            .def("get", &TickComponent::get, py::return_value_policy::reference);
    }
};
PyType<TickComponent, PyTickComponent, ComponentWrapperBase> pytickcomponent;