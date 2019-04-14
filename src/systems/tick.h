/*
    tick.h: tick system
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
#ifndef _systems_tick_h
#define _systems_tick_h

#include "component.h"
#include <functional>

using TickCallback = std::function<void(double)>;

class TickSystem {
public:
    using ComponentType = TickCallback;
    using IndexType = SlotMapIndex;

    TickSystem();

    IndexType create(TickCallback callback);
    TickCallback& get(const IndexType& i);
    void remove(const IndexType& i);

    void update(double dt);
    void earlyCleanup();
    static std::shared_ptr<TickSystem> instance;

private:
    SlotMap<TickCallback> tickCallbacks;
};

using TickComponent = ComponentWrapper<TickSystem>;

#endif // _systems_tick_h