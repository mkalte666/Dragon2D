/*
    rect.h: rect interface 
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
#ifndef _util_rect_h
#define _util_rect_h

#include <glm/vec2.hpp>

template <class T>
struct GenericRect {
    using ValueType = T;
    using VecType = glm::vec<2, ValueType>;
    GenericRect()
        : x(0)
        , y(0)
        , w(0)
        , h(0)
    {
    }
    GenericRect(ValueType x, ValueType y)
        : x(x)
        , y(y)
        , w(0)
        , h(0)
    {
    }
    GenericRect(ValueType x, ValueType y, ValueType w, ValueType h)
        : x(x)
        , y(y)
        , w(w)
        , h(h)
    {
    }
    GenericRect(const VecType& pos, const VecType& size)
        : x(pos.x)
        , y(pos.y)
        , w(size.x)
        , h(size.y)
    {
    }

    VecType pos() const
    {
        return VecType(x, y);
    }

    VecType size() const
    {
        return VecType(w, h);
    }

    ValueType left() const
    {
        return x;
    }

    ValueType right() const
    {
        return x + w;
    }

    ValueType top() const
    {
        return y;
    }

    ValueType bottom() const
    {
        return y + h;
    }

    bool intersect(const GenericRect& other) const
    {
        return !(left() > other.right() || right() < other.left() || top() > other.bottom() || bottom() < other.top());
    }

    GenericRect& operator+=(const VecType& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    friend GenericRect operator+(GenericRect rect, const VecType& other)
    {
        rect += other;
        return *this;
    }

    GenericRect& operator-=(const VecType& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    friend GenericRect operator-(GenericRect rect, const VecType& other)
    {
        rect -= other;
        return *this;
    }

    ValueType x = 0;
    ValueType y = 0;
    ValueType w = 0;
    ValueType h = 0;
};

using Rect = GenericRect<int32_t>;
using FRect = GenericRect<float>;
using DRect = GenericRect<double>;

#endif //_util_rect_h