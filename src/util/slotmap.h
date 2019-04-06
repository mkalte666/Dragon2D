/*
    slotmap.h: a slot map implementation
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
#ifndef _slotmap_h
#define _slotmap_h

#include <array>
#include <iterator>
#include <memory>
#include <type_traits>
#include <vector>

struct SlotMapIndex {
    SlotMapIndex() = default;
    SlotMapIndex(uint64_t intIndex)
    {
        index = static_cast<uint32_t>(intIndex & 0x00000000FFFFFFFF);
        generation = static_cast<uint32_t>((intIndex >> 32) & 0x00000000FFFFFFFF);
    }

    uint32_t generation;
    uint32_t index;
    bool operator==(const SlotMapIndex& other) const
    {
        return index == other.index && generation == other.generation;
    }
    bool operator<(const SlotMapIndex& other) const
    {
        return index < other.index;
    }
    bool operator>(const SlotMapIndex& other) const
    {
        return index > other.index;
    }
    uint64_t toInt() const
    {
        return (static_cast<uint64_t>(generation) << 32 | static_cast<uint64_t>(index));
    }
};

namespace std {
template <>
struct hash<SlotMapIndex> {
    typedef SlotMapIndex argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& s) const noexcept
    {
        return s.toInt();
    }
};
}

template <class T, size_t baseSize = 256>
class SlotMap {
public:
    using DataType = T;
    struct StorageType {
        StorageType()
            : free(false)
            , generation(0)
            , data()
            , ptr(reinterpret_cast<T*>(&data))
        {
        }
        bool free;
        uint32_t generation;
        typename std::aligned_storage<sizeof(T), alignof(T)>::type data;
        T* ptr;
    };
    struct Chunk {
        std::array<StorageType, baseSize> data;
        size_t size = 0;
    };
    using ChunkArray = std::vector<std::unique_ptr<Chunk>>;
    using FreeList = std::vector<int>;
    using IndexType = SlotMapIndex;

    struct iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        iterator() {}
        iterator(const ChunkArray* storage, uint32_t index, int inc = 1)
            : storage(storage)
            , index(index)
            , inc(inc)
        {
        }

        iterator& operator++()
        {
            auto maxIndex = baseSize * (storage->size() - 1) + storage->back()->size;
            do {
                index += inc;
            } while (
                index >= 0 && index < maxIndex && (*storage)[index / baseSize]->data[index % baseSize].free);
            return *this;
        }

        iterator operator++(int)
        {
            auto t = *this;
            auto maxIndex = baseSize * (storage->size() - 1) + storage->back()->size;
            do {
                index += inc;
            } while (
                index >= 0 && index < maxIndex && (*storage)[index / baseSize]->data[index % baseSize].free);
            return t;
        }

        iterator& operator--()
        {
            auto maxIndex = baseSize * (storage->size() - 1) + storage->back()->size;
            do {
                index -= inc;
            } while (
                index >= 0 && index < maxIndex && (*storage)[index / baseSize]->data[index % baseSize].free);
            return *this;
        }

        iterator operator--(int)
        {
            auto t = *this;
            auto maxIndex = baseSize * (storage->size() - 1) + storage->back()->size;
            do {
                index -= inc;
            } while (
                index >= 0 && index < maxIndex && (*storage)[index / baseSize]->data[index % baseSize].free);
            return t;
        }

        bool operator<(const iterator& other) const
        {
            return index < other.index;
        }
        bool operator<(size_t other) const
        {
            return index < other;
        }
        bool operator>(const iterator& other) const
        {
            return index > other.index;
        }
        bool operator>(size_t other) const
        {
            return index > other;
        }
        bool operator==(const iterator& other) const
        {
            return index == other.index;
        }
        bool operator==(size_t other) const
        {
            return index == other;
        }
        bool operator!=(const iterator& other) const
        {
            return index != other.index;
        }
        bool operator!=(size_t other) const
        {
            return index != other;
        }

        T& operator*()
        {
            return *reinterpret_cast<T*>(&(*storage)[index / baseSize]->data[index % baseSize].data);
        }

        T* operator->()
        {
            return reinterpret_cast<T*>(&(*storage)[index / baseSize]->data[index % baseSize].data);
        }

        StorageType& getTag() const
        {
            return (*storage)[index / baseSize]->data[index % baseSize];
        }

        IndexType getGenerationIndex() const
        {
            IndexType i;
            i.index = index;
            i.generation = getTag().generation;
            return i;
        }

        uint32_t getRawIndex() const
        {
            return index;
        }

    private:
        const ChunkArray* storage;
        uint32_t index;
        int inc;
    };

    SlotMap()
    {
        freelist.reserve(baseSize);
        data.emplace_back(new Chunk());
        currentChunk = 0;
    }

    ~SlotMap()
    {
        clear();

        data.clear();
    }

    iterator begin() const
    {
        iterator iter(&data, 0, 1);
        if (iter.getTag().free) {
            iter++; // automatically goes to first free or end()
        }

        return iter;
    }

    iterator end() const
    {
        return iterator(&data, static_cast<uint32_t>(currentChunk * baseSize + data[currentChunk]->size), 1);
    }

    iterator rbegin() const
    {

        iterator iter(&data, currentChunk * baseSize + data[currentChunk]->size, -1);
        if (iter.getTag().free) {
            iter++; // automatically goes to first free or end()
        }

        return iter;
    }

    iterator rend() const
    {
        iterator iter(&data, -1, 1);
        return iter;
    }

    T& operator[](const IndexType& index)
    {
        return *reinterpret_cast<T*>(&data[index.index / baseSize]->data[index.index % baseSize].data);
    }
    T& operator[](uint64_t intIndex)
    {
        IndexType index(intIndex);
        return *reinterpret_cast<T*>(&data[index.index / baseSize]->data[index.index % baseSize].data);
    }

    iterator find(const IndexType& index) const
    {
        if (index.index >= currentChunk * baseSize + data[currentChunk]->size) {
            return end();
        }

        auto& tag = data[index.index / baseSize]->data[index.index % baseSize];
        if (tag.free || tag.generation != index.generation) {
            return end();
        }

        return iterator(&data, index.index, 1);
    }

    IndexType insert(const T& value)
    {

        iterator iter = allocateTag();
        new (&iter.getTag().data) T(value);

        return iter.getGenerationIndex();
    }

    IndexType insert(T&& value)
    {
        iterator iter = allocateTag();
        new (&iter.getTag().data) T(std::move(value));
        return iter.getGenerationIndex();
    }

    template <typename... Args>
    IndexType emplace(Args&&... args)
    {
        iterator iter = allocateTag();
        new (&iter.getTag().data) T(std::forward<Args>(args)...);
        return iter.getGenerationIndex();
    }

    void remove(const IndexType& index)
    {
        auto iter = find(index);
        if (iter == end()) {
            return;
        }

        auto& tag = iter.getTag();
        reinterpret_cast<T*>(&tag.data)->~T();
        ++tag.generation;
        tag.free = true;
        freelist.push_back(iter.getRawIndex());
    }

    void clear()
    {
        for (auto&& chunk : data) {
            for (size_t i = 0; i < chunk->size; i++) {
                auto& elem = chunk->data[i];
                if (!elem.free) {
                    reinterpret_cast<T*>(&elem.data)->~T();
                }
            }
            // simple set size to 0
            chunk->size = 0;
        }

        currentChunk = 0;
        freelist.clear();
        freelist.reserve(baseSize);
    }

    // Clear, but does NOT CALL ELEMENT DESTRUCTORS
    // Only use for plain old data where you dont care
    // And really want a *fast* clear
    void fastClear()
    {
        for (auto&& chunk : data) {
            chunk->size = 0;
        }

        currentChunk = 0;
        freelist.clear();
    }

protected:
    iterator allocateTag()
    {
        if (!freelist.empty()) {
            iterator iter(&data, freelist.back());
            freelist.pop_back();
            iter.getTag().free = false;
            return iter;
        }

        // chunk full?
        if (data[currentChunk]->size >= baseSize) {
            data.emplace_back(new Chunk());
            ++currentChunk;
        }

        auto& chunk = data[currentChunk];

        // return new tag
        auto& tag = chunk->data[chunk->size];
        iterator iter(&data, static_cast<uint32_t>((data.size() - 1) * baseSize + chunk->size));
        ++data[currentChunk]->size;
        // placement new the data tag
        new (&tag) StorageType();

        return iter;
    }

private:
    ChunkArray data;
    size_t currentChunk;
    FreeList freelist;
};

#endif //_slotmap_h