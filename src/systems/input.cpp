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
#include <tinyxml2.h>
namespace xml = tinyxml2;
#include <imgui.h>

#include "runtime/filename.h"
#include "util/xmlhelpers.h"

std::shared_ptr<InputSystem> InputSystem::instance(nullptr);

InputSystem::InputSystem()
{
    //SDL_assert(0 == SDL_SetRelativeMouseMode(SDL_TRUE));

    xml::XMLDocument doc;
    auto loadResult = doc.LoadFile(Filename::gameFile("inputs.xml").c_str());
    if (loadResult != xml::XML_SUCCESS) {
        SDL_Log("Cannot load input config!");
        return;
    }

    auto inputs = doc.FirstChildElement("inputs");
    if (!inputs) {
        return;
    }

    for (auto inputTag = inputs->FirstChildElement("input"); inputTag; inputTag = inputTag->NextSiblingElement("input")) {
        std::string newName = nullAwareAttr(inputTag->Attribute("name"));
        std::string eventName = nullAwareAttr(inputTag->Attribute("event"));
        std::string paramStr = nullAwareAttr(inputTag->Attribute("param"));
        std::string eventParamStr = nullAwareAttr(inputTag->Attribute("eventparam"));
        std::string rawParam = nullAwareAttr(inputTag->Attribute("rawparam"));

        if (newName.empty() || eventName.empty() || paramStr.empty()) {
            SDL_Log("Inputs must have both 'name', 'event' and 'param' set!");
            continue;
        }

        if (paramStr != "inherit" && eventParamStr.empty()) {
            SDL_Log("Inputs that do not inherit the base event parameters must have 'eventparam' set");
            continue;
        }

        XmlInput input;
        input.name = newName;
        input.inherit = paramStr == "inherit";
        if (!input.inherit) {
            input.newParam = std::stoi(paramStr);

            if (rawParam != "0" && rawParam != "true") {
                input.origParam = static_cast<int>(SDL_GetKeyFromName(eventParamStr.c_str()));
            } else {
                // fire and pray
                input.origParam = std::stoi(eventParamStr);
            }
        }

        loadedInputs.insert(std::make_pair(eventName, input));
    }
}

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
}

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

    // generate basic event
    std::vector<EventInfo> baseEvents;

    switch (e.type) {
    case SDL_KEYDOWN: {
        if (e.key.repeat) {
            break;
        }
        EventInfo baseEvent;
        baseEvent.name = "keydown";
        baseEvent.param = static_cast<int64_t>(e.key.keysym.sym);
        baseEvents.push_back(baseEvent);
        break;
    }
    case SDL_KEYUP: {
        EventInfo baseEvent;
        baseEvent.name = "keyup";
        baseEvent.param = static_cast<int64_t>(e.key.keysym.sym);
        baseEvents.push_back(baseEvent);
        break;
    }
    case SDL_MOUSEMOTION: {
        if (e.motion.xrel != 0) {
            EventInfo baseEvent;
            baseEvent.name = "mousex";
            baseEvent.param = e.motion.xrel;
            baseEvents.push_back(baseEvent);
        }
        if (e.motion.yrel != 0) {
            EventInfo baseEvent;
            baseEvent.name = "mousey";
            baseEvent.param = e.motion.yrel;
            baseEvents.push_back(baseEvent);
        }
        break;
    }
    case SDL_MOUSEBUTTONDOWN: {
        EventInfo baseEvent;
        baseEvent.name = "mousedown";
        baseEvent.param = static_cast<int64_t>(e.button.button);
        baseEvents.push_back(baseEvent);
    }
    case SDL_MOUSEBUTTONUP: {
        EventInfo baseEvent;
        baseEvent.name = "mouseup";
        baseEvent.param = static_cast<int64_t>(e.button.button);
        baseEvents.push_back(baseEvent);
    }
    default:
        break; // dont care
    }

    

    // check our own nice special type of events
    for (auto&& baseEvent : baseEvents) {
        processedEvents.push_back(baseEvent);
        auto relevantLoadedInputs = loadedInputs.equal_range(baseEvent.name);
        // for each own input type corresponing to the base one
        for (auto iter = relevantLoadedInputs.first; iter != relevantLoadedInputs.second; ++iter) {
            // do we inherit or are we what is marked with the second param?
            if (iter->second.inherit || baseEvent.param == iter->second.origParam) {
                EventInfo childEvent;
                childEvent.name = iter->second.name;
                childEvent.param = iter->second.newParam;
                // inherit means same param as base event
                if (iter->second.inherit) {
                    childEvent.param = baseEvent.param;
                }
                processedEvents.push_back(childEvent);
            }
        }
    }

    // call all relevant event handlers
    for (auto&& event : processedEvents) {
        for (auto&& input : inputs) {
            if (input.name == event.name) {
                try {

                    input.callback(event.name, event.param);
                } catch (std::exception e) {
                    SDL_Log("Exception in Callback for Input %s: %s", input.name.c_str(), e.what());
                }
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