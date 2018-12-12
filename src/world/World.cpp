#include "pch.h"
#include "World.h"

#include "../utils/Exceptions.h"

#include <algorithm>

namespace tk
{
World::World()
{
}

World::~World()
{
}

// public:
void World::loadColumn(const glm::ivec2 &at)
{
    if (findColumn(at) != m_columns.end())
        throw RuntimeException("Column is already loaded");

    auto newPair = std::make_pair(at, std::make_unique<ChunkColumn>());
    setNeighboors(&newPair, true);

    m_columns.push_back(std::move(newPair));
    // Add and get last
    auto &last = m_columns.back();

    auto it = findColumn({at.x + 1, at.y});
    if (it != m_columns.end())
    {
        setNeighboors(it.base(), true);
        it->second->generateMeshes(it->first);
    }

    if ((it = findColumn({at.x - 1, at.y})) != m_columns.end())
    {
        setNeighboors(it.base(), true);
        it->second->generateMeshes(it->first);
    }

    if ((it = findColumn({at.x, at.y + 1})) != m_columns.end())
    {
        setNeighboors(it.base(), true);
        it->second->generateMeshes(it->first);
    }

    if ((it = findColumn({at.x, at.y - 1})) != m_columns.end())
    {
        setNeighboors(it.base(), true);
        it->second->generateMeshes(it->first);
    }

    last.second->generateMeshes(at);
}

void World::unloadColumn(const glm::ivec2 &at)
{
    auto it = findColumn(at);

    if (it == m_columns.end())
        throw RuntimeException("Column is already not loaded");

    m_columns.erase(it);
}

ChunkColumn *World::getColumn(const glm::ivec2 &at)
{
    auto it = findColumn(at);

    if (it == m_columns.end())
        throw RuntimeException("Can't find column in list");

    return it->second.get();
}

std::vector<RenderData> World::getRenderData() noexcept
{
    std::vector<RenderData> toReturn;

    for (auto &c : m_columns)
    {
        auto d = c.second->getRenderData();
        toReturn.insert(toReturn.end(), d.begin(), d.end());
    }

    return toReturn;
}

// private:

std::vector<WorldColumn>::iterator World::findColumn(const glm::ivec2 &at)
{
    auto it = std::find_if(
        m_columns.begin(),
        m_columns.end(),
        [&](const WorldColumn &current) {
            return current.first == at;
        });

    return it;
}

void World::setNeighboors(WorldColumn *ofColumn, bool presenceStatus) noexcept
{
    ChunkColumn *left = nullptr;
    ChunkColumn *right = nullptr;
    ChunkColumn *front = nullptr;
    ChunkColumn *back = nullptr;

    ChunkColumn *valueToSet = presenceStatus ? ofColumn->second.get() : nullptr;

    if (presenceStatus)
    {
        auto temp = findColumn({ofColumn->first.x - 1, ofColumn->first.y});
        if (temp != m_columns.end())
            left = temp->second.get();

        temp = findColumn({ofColumn->first.x + 1, ofColumn->first.y});
        if (temp != m_columns.end())
            right = temp->second.get();

        temp = findColumn({ofColumn->first.x, ofColumn->first.y + 1});
        if (temp != m_columns.end())
            front = temp->second.get();

        temp = findColumn({ofColumn->first.x, ofColumn->first.y - 1});
        if (temp != m_columns.end())
            back = temp->second.get();
    }

    if (left)
        left->adjacentColumns[1] = valueToSet; // Set his right
    if (right)
        right->adjacentColumns[0] = valueToSet; // Set his left
    if (front)
        front->adjacentColumns[3] = valueToSet; // Set his back
    if (back)
        back->adjacentColumns[2] = valueToSet; // Set his front

    // Set this column neighboors

    ofColumn->second->adjacentColumns[0] = left;
    ofColumn->second->adjacentColumns[1] = right;
    ofColumn->second->adjacentColumns[2] = front;
    ofColumn->second->adjacentColumns[3] = back;
}

} // namespace tk
