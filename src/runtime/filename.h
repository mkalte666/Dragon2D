/*
    filename.h: filename wrangling 
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
#ifndef _filename_h
#define _filename_h

#include <string>

struct Filename
{
    static std::string engineFile(const std::string& file);
    static std::string gameFile(const std::string& file);
    static std::string configFile(const std::string& file);

    static std::string basePath;
    static std::string gamePath;
    static std::string writeablePath;
};



#endif // _filename_h