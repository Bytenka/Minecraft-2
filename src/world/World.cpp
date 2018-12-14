#include "pch.h"
#include "World.h"

#include "../utils/Exceptions.h"

#include <exception>
#include <algorithm>
#include <chrono>

namespace tk
{
World::World()
{
}

World::~World()
{
}

void World::updateLoop(World &world, const glm::dvec3 &playerPos, bool &shouldStop)
{
    while (!shouldStop)
    {
        world.update(playerPos);
    }
}

// public:
void World::loadColumn(const glm::ivec2 &at)
{
    auto it = std::find_if(
        m_columns.begin(),
        m_columns.end(),
        [&](const WorldColumn &current) {
            return current.first == at;
        });

    if (it != m_columns.end())
        throw std::invalid_argument("Column is already loaded");

    if (std::find(m_toLoadColumns.begin(), m_toLoadColumns.end(), at) != m_toLoadColumns.end())
        throw std::invalid_argument("Column is already queued to be loaded");

    auto it2 = std::find(m_toUnloadColumns.begin(), m_toUnloadColumns.end(), at);

    if (it2 != m_toUnloadColumns.end())
    {
        m_toUnloadColumns.erase(it2);
        return;
    }

    m_toLoadColumns.push_back(at);
}

void World::unloadColumn(const glm::ivec2 &at)
{
    auto it = std::find_if(
        m_columns.begin(),
        m_columns.end(),
        [&](const WorldColumn &current) {
            return current.first == at;
        });

    if (it == m_columns.end())
        throw std::invalid_argument("Column is already unloaded");

    if (std::find(m_toUnloadColumns.begin(), m_toUnloadColumns.end(), at) != m_toUnloadColumns.end())
        throw std::invalid_argument("Column is already queued to be unloaded");

    auto it2 = std::find(m_toLoadColumns.begin(), m_toLoadColumns.end(), at);

    if (it2 != m_toLoadColumns.end())
    {
        m_toLoadColumns.erase(it2);
        return;
    }

    m_toUnloadColumns.push_back(at);
}

ChunkColumn *World::getColumn(const glm::ivec2 &at)
{
    auto it = findColumn(at);

    if (it == m_columns.end())
    {
        auto it2 = std::find(m_toLoadColumns.begin(), m_toLoadColumns.end(), at);
        if (it2 != m_toLoadColumns.end())
            throw RuntimeException("Column is still waiting to be loaded");
        else
            throw RuntimeException("Can't find column in list");
    }

    return it->second.get();
}

const std::vector<RenderData> &World::getRenderData() noexcept
{
    if (m_mainMutex.try_lock())
    {
        poolUnload();

        m_drawCache.clear();
        for (auto &c : m_columns)
        {
            auto d = c.second->getRenderData();
            m_drawCache.insert(m_drawCache.end(), d.begin(), d.end());
        }
        m_mainMutex.unlock();
    }

    return m_drawCache;
}

// private:

void World::update(const glm::dvec3 &playerPos)
{
    m_mainMutex.lock();

    bool hasLoaded = poolLoad();

    m_mainMutex.unlock();

    if (hasLoaded)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    else
        std::this_thread::sleep_for(std::chrono::seconds(1));
}

bool World::poolLoad()
{
    if (m_toLoadColumns.empty())
        return false;

    auto current = m_toLoadColumns.front();
    auto newPair = std::make_pair(current, std::make_unique<ChunkColumn>());
    // Add and get last
    m_columns.push_back(std::move(newPair));
    auto &last = m_columns.back();

    setNeighboors(&last, true);
    rebuildNeighboors(&last);
    last.second->generateMeshes(current);

    m_toLoadColumns.pop_front();
    LOG_TRACE("Fully loaded column {}, {}", current.x, current.y);
    return true;
}

bool World::poolUnload()
{
    if (m_toUnloadColumns.empty())
        return false;

    auto current = m_toUnloadColumns.front();
    auto it = findColumn(current);

    m_columns.erase(it);
    m_toUnloadColumns.pop_front();

    LOG_TRACE("Fully unloaded column {}, {}", current.x, current.y);
    return true;
}

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
        auto &pos = ofColumn->first;

        auto lambda = [&](const std::vector<WorldColumn>::iterator &it, ChunkColumn **toSet) {
            if (it != m_columns.end())
                *toSet = it->second.get();
        };

        lambda(findColumn({pos.x - 1, pos.y}), &left);
        lambda(findColumn({pos.x + 1, pos.y}), &right);
        lambda(findColumn({pos.x, pos.y + 1}), &front);
        lambda(findColumn({pos.x, pos.y - 1}), &back);
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

void World::rebuildNeighboors(WorldColumn *ofChunk)
{
    auto &pos = ofChunk->first;

    auto lambda = [&](const std::vector<WorldColumn>::iterator &it) {
        if (it != m_columns.end())
            it->second->generateMeshes(it->first);
    };

    lambda(findColumn({pos.x + 1, pos.y}));
    lambda(findColumn({pos.x - 1, pos.y}));
    lambda(findColumn({pos.x, pos.y + 1}));
    lambda(findColumn({pos.x, pos.y - 1}));
}
} // namespace tk
