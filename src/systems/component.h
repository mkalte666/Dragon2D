/*
    component.h: generic component wrapper
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
#ifndef _component_h
#define _component_h

#include "python/python.h"
#include "util/slotmap.h"

class ComponentWrapperBase {
public:
    virtual ~ComponentWrapperBase() = default;
};
using ComponentWrapperBasePtr = std::shared_ptr<ComponentWrapperBase>;

PYBIND11_MAKE_OPAQUE(std::vector<ComponentWrapperBasePtr>);

template <class SystemClass>
class ComponentWrapper : public ComponentWrapperBase {
public:
    using ComponentType = typename SystemClass::ComponentType;
    using IndexType = typename SystemClass::IndexType;
    using Ptr = typename std::shared_ptr<ComponentWrapper>;

    template <typename... Args>
    ComponentWrapper(Args&&... args)
    {
        index = SystemClass::instance->create(std::forward<Args>(args)...);
        valid = true;
    }

    ComponentWrapper(ComponentWrapper&& other) = delete;

    ComponentWrapper(const ComponentWrapper& other) = delete;

    ~ComponentWrapper()
    {
        SystemClass::instance->remove(index);
    }

    ComponentType& get()
    {
        return SystemClass::instance->get(index);
    }

    IndexType getIndex() const
    {
        return index;
    }

private:
    IndexType index;
    bool valid;
};

#endif //_component_h