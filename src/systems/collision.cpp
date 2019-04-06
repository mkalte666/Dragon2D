/*
    collision.cpp: collision system
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
#include "collision.h"

class PyCollider {
public:
    static void initModule(py::module& m)
    {
        py::class_<Collider> c(m, "Collider");
        c.def_readwrite("aabb", &Collider::aabb);
        c.def_readwrite("mask", &Collider::mask);
    }
};
PyType<Collider, PyCollider, FRect> pycollider;

struct CollisionGrid {
    static const int64_t maxSize = 100000000; // there be enough space
    using CellMap = SlotMap<SlotMapIndex, 32>;
    float cellSize = 100.0f;

    int64_t cell(const glm::vec2& pos)
    {
        int64_t x = static_cast<int64_t>(pos.x / cellSize) % maxSize;
        int64_t y = static_cast<int64_t>(pos.y / cellSize) % maxSize;
        int64_t index = x + y * maxSize;

        return index;
    }

    void insert(const Collider& c, const SlotMapIndex& i)
    {
        const auto& transform = TransformSystem::instance->get(c.transformId);
        FRect actualRect = c.aabb + transform.position;
        auto c1 = cell(actualRect.topLeft());
        auto c2 = cell(actualRect.topRight());
        auto c3 = cell(actualRect.bottomLeft());
        auto c4 = cell(actualRect.bottomRight());

        grid[c1].insert(i);
        if (c1 != c2) {
            grid[c2].insert(i);
        }
        if (c1 != c3 && c2 != c3) {
            grid[c3].insert(i);
        }
        if (c1 != c4 && c2 != c4 && c3 != c4) {
            grid[c4].insert(i);
        }
    }

    void allCells(std::vector<int64_t>& target, const Collider& c)
    {
        const auto& transform = TransformSystem::instance->get(c.transformId);
        FRect actualRect = c.aabb + transform.position;
        auto c1 = cell(actualRect.topLeft());
        auto c2 = cell(actualRect.topRight());
        auto c3 = cell(actualRect.bottomLeft());
        auto c4 = cell(actualRect.bottomRight());

        target.reserve(4);
        target.push_back(c1);
        if (c1 != c2) {
            target.push_back(c2);
        }
        if (c1 != c3 && c2 != c3) {
            target.push_back(c3);
        }
        if (c1 != c4 && c2 != c4 && c3 != c4) {
            target.push_back(c4);
        }
    }

    void clear()
    {
        for (auto&& cell : grid) {
            cell.second.fastClear();
        }
    }

    std::map<int64_t, CellMap> grid;
};

struct CollisionSystemData {
    SlotMap<Collider> colliders;
    CollisionGrid grid;
};

std::shared_ptr<CollisionSystem> CollisionSystem::instance(nullptr);

CollisionSystem::CollisionSystem()
{
    data.reset(new CollisionSystemData);
}

CollisionSystem::~CollisionSystem()
{
    data.reset();
}

CollisionSystem::IndexType CollisionSystem::create(const TransformComponent& transform, const FRect& aabb, uint64_t mask)
{
    return create(transform.getIndex(), aabb);
}

CollisionSystem::IndexType CollisionSystem::create(const TransformSystem::IndexType& transformId, const FRect& aabb, uint64_t mask)
{
    Collider c;
    c.transformId = transformId;
    c.aabb = aabb;
    c.mask = mask;
    auto index = data->colliders.insert(c);
    return index;
}

Collider& CollisionSystem::get(const IndexType& i)
{
    return data->colliders[i];
}

void CollisionSystem::remove(const IndexType& i)
{
    data->colliders.remove(i);
}

void CollisionSystem::update(double dt)
{
    // TODO: maybe only update this on demand?
    data->grid.clear();
    for (auto iter = data->colliders.begin(); iter != data->colliders.end(); ++iter) {
        data->grid.insert(*iter, iter.getGenerationIndex());
    }
}

bool CollisionSystem::checkCollision(const IndexType& i)
{
    auto iter = data->colliders.find(i);
    if (iter == data->colliders.end()) {
        return false;
    }
    const auto& c = *iter;
    const auto& transformedAabb = c.aabb + TransformSystem::instance->get(c.transformId).position;

    std::vector<int64_t> cells;
    data->grid.allCells(cells, c);
    for (auto&& cell : cells) {
        for (const auto& otherIndex : data->grid.grid[cell]) {
            if (otherIndex == i) {
                continue;
            }
            auto otherIter = data->colliders.find(otherIndex);
            if (otherIter == data->colliders.end()) {
                continue;
            }
            const auto& other = *otherIter;
            // check mask and maybe continue before we fetch the other transform
            if (c.mask && other.mask && !(c.mask & other.mask)) {
                continue;
            }
            const auto& otherTransformedAabb = other.aabb + TransformSystem::instance->get(other.transformId).position;
            if (transformedAabb.intersect(otherTransformedAabb)) {
                return true;
            }
        }
    }

    return false;
}

class PyCollisionComponent {
public:
    static void initModule(py::module& m)
    {
        py::class_<CollisionComponent, CollisionComponent::Ptr, ComponentWrapperBase> c(m, "CollisionComponent");
        c
            .def(py::init<const TransformComponent&, const FRect&>())
            .def(py::init<const TransformComponent&, const FRect&, uint64_t>())
            .def("get", &CollisionComponent::get, py::return_value_policy::reference);
    }
};
PyType<CollisionComponent, PyCollisionComponent, Collider> pycollisioncomponent;