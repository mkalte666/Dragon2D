/*
    filename.cpp: filename wrangling 
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

#include "filename.h"
#include "python/python.h"

std::string Filename::basePath = "./";
std::string Filename::gamePath = "./";
std::string Filename::writeablePath = "./";

std::string Filename::engineFile(const std::string& file)
{
    return basePath + "../" + file;
}

std::string Filename::gameFile(const std::string& file)
{
    return gamePath + "/" + file;
}

std::string Filename::configFile(const std::string& file)
{
    return writeablePath + "/" + file;
}


// python interface
class PyFilename {
public:
    static void initModule(py::module& m) {
        py::class_<Filename> c(m, "Filename");
        c
            .def_static("engineFile", &Filename::engineFile)
            .def_static("gameFile", &Filename::gameFile)
            .def_static("configFile", &Filename::configFile)
            .def_readonly_static("basePath", &Filename::basePath)
            .def_readonly_static("gamePath", &Filename::gamePath)
            .def_readonly_static("writeablePath", &Filename::writeablePath);
    }
};
PyType<PyFilename> pyfilename;