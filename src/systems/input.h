/*
    input.h: input system 
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
#ifndef _systems_input_h
#define _systems_input_h

#include "systems/component.h"
#include <pybind11/functional.h>
#include <SDL.h>
#include <functional>
#include <string>

using InputFunction = std::function<void(const std::string& name, int64_t param)>;

struct Input {
    std::string name = "";
    InputFunction callback = nullptr;
};

class InputSystem {
public:
    InputSystem();
    using ComponentType = Input;
    using IndexType = SlotMapIndex;

    IndexType create(const std::string& name, const InputFunction& callback);
    IndexType create(const std::string& name, const py::function& callback);
    Input& get(const IndexType& i);
    void remove(const IndexType& i);

    void processEvent(const SDL_Event& e);
    void update(double dt);

    void earlyCleanup();

    static std::shared_ptr<InputSystem> instance;

private:
    struct XmlInput {
        std::string name = "";
        bool inherit = false;
        int origParam = 0;
        int newParam = 0;
    };

    SlotMap<Input> inputs;
    std::multimap<std::string, XmlInput> loadedInputs;
};

using InputComponent = ComponentWrapper<InputSystem>;

#endif //_systems_input_h