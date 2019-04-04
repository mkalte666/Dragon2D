/*
    input.cpp: input system 
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
#include "input.h"

#include <pybind11/functional.h>
#include <imgui.h>

std::shared_ptr<InputSystem> InputSystem::instance(nullptr);

InputSystem::IndexType InputSystem::create(const std::string& name, const InputFunction& callback)
{
    Input in;
    in.name = name;
    in.callback = callback;
    return inputs.insert(std::move(in));
}

Input& InputSystem::get(const IndexType& i)
{
    return inputs[i];
}

void InputSystem::remove(const IndexType& i)
{
    inputs.remove(i);
}d

void InputSystem::processEvent(const SDL_Event& e)
{
    // events we process out of this event go in here.
    // might be more than one
    // example: sdl events will always result in "keyup" and "keydown"
    //          but user might register "moveup" for one or both of these
    struct EventInfo {
        std::string name;
        int param;
    };

    std::vector<EventInfo> processedEvents;

    // generate basic events
    switch (e.type) {
    case SDL_KEYDOWN: {
        if (e.key.repeat) {
            break;
        }
        EventInfo i;
        i.name = "keydown";
        i.param = static_cast<int64_t>(e.key.keysym.scancode);
        processedEvents.push_back(i);
        break;
    }
    case SDL_KEYUP: {
        EventInfo i;
        i.name = "keyup";
        i.param = static_cast<int64_t>(e.key.keysym.scancode);
        processedEvents.push_back(i);
        break;
    }
    default:
        break; // dont care
    }

    // call all relevant event handlers
    for (auto&& event : processedEvents) {
        for (auto&& input : inputs) {
            if (input.name == event.name) {
                input.callback(event.name, event.param);
            }
        }
    }
}

void InputSystem::update(double dt)
{
}

void InputSystem::earlyCleanup()
{
    inputs.clear();
}

class PyInput {
public:
    static void initModule(py::module& m)
    {
        py::class_<Input> c(m, "Input");
        c
            .def(py::init<>())
            .def_readwrite("name", &Input::name)
            .def_readwrite("callback", &Input::callback);
    }
};
PyType<Input, PyInput> pyinput;

class PyInputComponent {
public:
    static void initModule(py::module& m)
    {
        py::class_<InputComponent, InputComponent::Ptr, ComponentWrapperBase> c(m, "InputComponent");
        c
            .def(py::init<const std::string, const InputFunction&>())
            .def("get", &InputComponent::get, py::return_value_policy::reference);
    }
};

PyType<InputComponent, PyInputComponent, ComponentWrapperBase, Input> pyinputcomponent;