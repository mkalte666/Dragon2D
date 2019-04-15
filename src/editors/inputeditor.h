/*
    inputeditor.h: input editor 
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
#ifndef _editors_inputeditor_h
#define _editors_inputeditor_h

#include "editor.h"

class InputEditor : public Editor {
public:
    InputEditor();
    virtual void update(double dt) override;
    virtual void menu() override;

private:
    bool showInputEditor = false;
};

#endif //_editors_inputeditor_h