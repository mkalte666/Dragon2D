set(runtimeSources 
    runtime/main.cpp
    runtime/filename.h
    runtime/filename.cpp)

set(pythonSources
    python/commonbindings.cpp
    python/python.cpp
    python/python.h)

set(systemSources 
    systems/entity.cpp
    systems/entity.h
    systems/entitymanager.cpp
    systems/entitymanager.h
    systems/init.cpp
    systems/init.h
    systems/component.h
    systems/position.cpp
    systems/position.h)

set(utilSources
    util/slotmap.h)

set(d2dSources 
    ${runtimeSources}
    ${pythonSources}
    ${systemSources}
    ${utilSources})

add_executable(d2d ${d2dSources})
target_include_directories(d2d PRIVATE ${CMAKE_PROJECT_DIR}/src/)
target_link_libraries(d2d PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_IMAGE tinyxml2 imgui pybind11::embed glm)