/*
    editor.h: Editor base class
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
#ifndef _editors_editor_h
#define _editors_editor_h 

#include <memory>
#include <string>

class Editor {
public:
    using Ptr = std::shared_ptr<Editor>;
    virtual void update(double dt) {}
    virtual void menu() {}
    std::string name = "";
};

#endif //_editors_editor_h