/*
    camera.h: camera interface 
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
#ifndef _systems_camera_h
#define _systems_camera_h

#include "component.h"
#include "transform.h"
#include "util/rect.h"

struct Camera {
    TransformSystem::IndexType transformId = TransformSystem::IndexType();
    glm::vec2 offset = glm::vec2(0.0);
    bool fillTarget = true;
    Rect viewport = Rect();
};

class CameraSystem {
public:
    using ComponentType = Camera;
    using IndexType = SlotMapIndex;
    CameraSystem();
    ~CameraSystem();

    IndexType create(const TransformComponent& transformComponent, bool fillTarget = true, const Rect& viewport = Rect());
    IndexType create(const TransformSystem::IndexType& transformId, bool fillTarget = true, const Rect& viewport = Rect());
    Camera& get(const IndexType& i);
    void remove(const IndexType& i);

	const SlotMap<Camera, 32>& getCameras() const;

    static std::shared_ptr<CameraSystem> instance;

private:
    SlotMap<Camera, 32> cameras;
};

using CameraComponent = ComponentWrapper<CameraSystem>;

#endif //_systems_camera_h