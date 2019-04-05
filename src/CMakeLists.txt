set(runtimeSources 
    runtime/main.cpp
    runtime/filename.h
    runtime/filename.cpp
    runtime/window.cpp
    runtime/window.h)

set(pythonSources
    python/commonbindings.cpp
    python/python.cpp
    python/python.h)

set(systemSources 
	systems/camera.cpp
	systems/camera.h
    systems/entity.cpp
    systems/entity.h
    systems/entitymanager.cpp
    systems/entitymanager.h
    systems/init.cpp
    systems/init.h
	systems/input.cpp
	systems/input.h
    systems/component.cpp
    systems/component.h
	systems/simplephysics.cpp
	systems/simplephysics.h
    systems/sprite.cpp
    systems/sprite.h
    systems/tilemap.cpp
    systems/tilemap.h
    systems/transform.cpp
    systems/transform.h)

set(utilSources
	util/rect.cpp
	util/rect.h
    util/slotmap.h
	util/xmlhelpers.h
    util/tiled/tmx.cpp
    util/tiled/tmx.h
    util/tiled/tsx.cpp
    util/tiled/tsx.h)

set(d2dSources 
    ${runtimeSources}
    ${pythonSources}
    ${systemSources}
    ${utilSources})

# does this work?
if (WIN32)
	list(APPEND utilSources util/slotmap.natvis)
endif(WIN32)

add_executable(d2d ${d2dSources})
target_include_directories(d2d PRIVATE ${CMAKE_PROJECT_DIR}/src/)
target_link_libraries(d2d PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_IMAGE tinyxml2 imgui pybind11::embed glm)