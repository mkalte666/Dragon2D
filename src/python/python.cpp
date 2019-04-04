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
#include "SDL.h"
#include <functional>
#include <map>
#include <vector>

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

void PyClassList::initAll(py::module& m)
{
    using funcType = std::function<void()>;
    std::map<std::string, PyClassList*> loaders;
    std::vector<std::string> loadedClasses;

    // load classes into the map
    for (PyClassList* p = list; p != nullptr; p = p->next) {
        loaders[p->getName()] = p;
    }

    // recursivly load dependencies
    std::function<void(std::pair<std::string, PyClassList*>)> recursiveLoad
        = [&](std::pair<std::string, PyClassList*> pair) -> void {
        // did already load this one
        if (std::find(loadedClasses.begin(), loadedClasses.end(), pair.first) != loadedClasses.end()) {
            return;
        }
        loadedClasses.push_back(pair.first);

        for (auto&& depends : pair.second->getDependencies()) {
            // dependency exsists, so load it
            if (auto iter = loaders.find(depends); iter != loaders.end()) {
                recursiveLoad(*iter);
            } else {
                SDL_Log("WARNING: Python type loading problem\n"
                        "   Type '%s' depends on unknown type '%s'",
                    pair.first.c_str(),
                    depends.c_str());
            }
        }

        // try loading in any case
        try {
            pair.second->initModule(m);
        } catch (std::exception e) {
            SDL_Log("Error during class loading for pybind11: %s", e.what());
        }
    };

    // load them all
    for (auto&& pair : loaders) {
        recursiveLoad(pair);
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
