/*
    camera.cpp: camera system 
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
#include "camera.h"

std::shared_ptr<CameraSystem> CameraSystem::instance(nullptr);

CameraSystem::CameraSystem()
    : cameras()
{
}

CameraSystem::~CameraSystem()
{
}

CameraSystem::IndexType CameraSystem::create(const TransformComponent& transformComponent, bool centered, bool fillTarget, const Rect& viewport)
{
    return create(transformComponent.getIndex(), centered, fillTarget, viewport);
}

CameraSystem::IndexType CameraSystem::create(const TransformSystem::IndexType& transformId, bool centered, bool fillTarget, const Rect& viewport)
{
    Camera c;
    c.transformId = transformId;
    c.ceneterd = centered;
    c.fillTarget = fillTarget;
    c.viewport = viewport;
    return cameras.insert(std::move(c));
}

Camera& CameraSystem::get(const IndexType& i)
{
    return cameras[i];
}

void CameraSystem::remove(const IndexType& i)
{
    cameras.remove(i);
}

const SlotMap<Camera, 32>& CameraSystem::getCameras() const
{
    return cameras;
}

class PyCamera {
public:
    static void initModule(py::module& m)
    {
        py::class_<Camera> c(m, "Camera");
        c
            .def(py::init<>())
            .def_readwrite("offset", &Camera::offset)
            .def_readwrite("centered", &Camera::ceneterd)
            .def_readwrite("fillTarget", &Camera::fillTarget)
            .def_readwrite("viewport", &Camera::viewport);
    }
};
PyType<Camera, PyCamera, glm::vec2, Rect> pycamera;

class PyCameraComponent {
public:
    static void initModule(py::module& m)
    {
        py::class_<CameraComponent, CameraComponent::Ptr, ComponentWrapperBase> c(m, "CameraComponent");
        c
            .def(py::init<const TransformComponent&>())
            .def(py::init<const TransformComponent&, bool>())
            .def(py::init<const TransformComponent&, bool, bool, const Rect&>())
            .def("get", &CameraComponent::get, py::return_value_policy::reference);
    }
};
PyType<CameraComponent, PyCameraComponent, ComponentWrapperBase, Camera> pycameracomponent;