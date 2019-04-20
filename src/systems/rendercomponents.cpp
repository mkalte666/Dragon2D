/*
    rendercomponents.cpp: rendering componennts
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

#include "rendercomponents.h"

SpriteComponent::~SpriteComponent()
{
    RenderSystem::instance->removeSprite(index);
}

Sprite& SpriteComponent::get()
{
    return RenderSystem::instance->getSprite(index);
}

SpriteComponent::IndexType SpriteComponent::getIndex() const
{
    return index;
}

class PySpriteComponent {
public:
    static void initModule(py::module& m)
    {
        py::class_<SpriteComponent, SpriteComponent::Ptr, ComponentWrapperBase> c(m, "SpriteComponent");
        c
            .def(py::init<const TransformComponent&, const std::string&, uint8_t>())
            .def("get", &SpriteComponent::get, py::return_value_policy::reference);
    }
};
PyType<SpriteComponent, PySpriteComponent, ComponentWrapperBase, Sprite> pyspritecomponent;

SpriteBatchComponent::~SpriteBatchComponent()
{
    RenderSystem::instance->removeSprite(index);
}

SpriteBatch& SpriteBatchComponent::get()
{
    return RenderSystem::instance->getBatch(index);
}

SpriteBatchComponent::IndexType SpriteBatchComponent::getIndex() const
{
    return index;
}
