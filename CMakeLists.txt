#    CMakeLists.txt 
#
#    Copyright (C) 2019  Malte Kießling
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    any later version.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.
cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)
set (CMAKE_CXX_STANDARD 17)
# disable in-source build. found at https://stackoverflow.com/questions/1208681/with-cmake-how-would-you-disable-in-source-builds
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(dragon2d)



# put stuff into "bin" and "lib" because we can 
# sauce: https://stackoverflow.com/questions/7747857/in-cmake-how-do-i-work-around-the-debug-and-release-directories-visual-studio-2
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/buildpackage/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/buildpackage/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/buildpackage/bin)
set(DOC_OUTPUT_DIR ${CMAKE_BINARY_DIR}/buildpackage/doc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

# this is for the find package n stuff.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

add_subdirectory(3rdparty/imgui-cmake-blob)
add_subdirectory(3rdparty/pybind11)
add_subdirectory(3rdparty/glm)
# fix NOMINMAX bug :( 
if (WIN32)
add_definitions(-DNOMINMAX)
endif(WIN32)

# windows we want the cplusplus macro and /MP
if (WIN32)
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /MP /Zc:__cplusplus")
endif(WIN32)

find_package(SDL2 REQUIRED)
find_package(SDL2_IMAGE REQUIRED)
find_package(Python COMPONENTS Interpreter Development REQUIRED)

add_subdirectory(3rdparty/tinyxml2)

include_directories(
	${PROJECT_SOURCE_DIR}/src 
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR})
# now we have all the base libs. i hope. 

add_executable(dummymain src/dummy/dummymain.cpp)
target_link_libraries(dummymain SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_IMAGE tinyxml2 imgui pybind11::embed)

add_subdirectory(src)

# dll blob 
if(WIN32)
    file(GLOB PYTHON_DLL_FILES ${Python_RUNTIME_LIBRARY_DIRS}/*.*)
    message("Python dlls will come from ${Python_RUNTIME_LIBRARY_DIRS}")
    set(COPY_DLL_LIST 
        ${SDL2_RUNTIME_LIBRARY}
        ${SDL2_IMAGE_RUNTIME_LIBRARY_ADDITIONALS}
        ${PYTHON_DLL_FILES})
    foreach(filename ${COPY_DLL_LIST})
        get_filename_component(filename_only ${filename} NAME)
        message("Copying library file to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${filename_only}")
        configure_file(${filename} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${filename_only} COPYONLY)
    endforeach(filename)
endif(WIN32)