#include "pch.h"
#include "World.h"

#include "../utils/Exceptions.h"

#include <GLFW/glfw3.h>

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
	m_mainMutex.lock();

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
		m_mainMutex.unlock();
        return;
    }

    m_toLoadColumns.push_back(at);

	m_mainMutex.unlock();
}

void World::unloadColumn(const glm::ivec2 &at)
{
	m_mainMutex.lock();

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
		m_mainMutex.unlock();
        return;
    }

    m_toUnloadColumns.push_back(at);

	m_mainMutex.unlock();
}

bool World::canLoadColumn(const glm::ivec2 &at) const
{
    auto it1 = std::find(m_toLoadColumns.begin(), m_toLoadColumns.end(), at);
    if (it1 != m_toLoadColumns.end())
        return false;

    auto it2 = std::find(m_toUnloadColumns.begin(), m_toUnloadColumns.end(), at);
    if (it2 != m_toUnloadColumns.end())
        return false;

    auto it3 = std::find_if(
        m_columns.begin(),
        m_columns.end(),
        [&](const WorldColumn &current) {
            return current.first == at;
        });

    if (it3 != m_columns.end())
        return false;

    return true;
}

bool World::canUnloadColumn(const glm::ivec2 &at) const
{
	auto it1 = std::find(m_toLoadColumns.begin(), m_toLoadColumns.end(), at);
	if (it1 != m_toLoadColumns.end())
		return false;

	auto it2 = std::find(m_toUnloadColumns.begin(), m_toUnloadColumns.end(), at);
	if (it2 != m_toUnloadColumns.end())
		return false;

	auto it3 = std::find_if(
		m_columns.begin(),
		m_columns.end(),
		[&](const WorldColumn &current) {
		return current.first == at;
	});

	if (it3 == m_columns.end())
		return false;

	return true;
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

		//if (m_toLoadColumns.empty())
			//m_columns.clear();

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
	// Load
	for (int x = playerPos.x / CHUNK_SIZE - 3; x < playerPos.x / CHUNK_SIZE + 3; x++)
		for (int z = playerPos.z / CHUNK_SIZE - 3; z < playerPos.z / CHUNK_SIZE + 3; z++)
		{
			glm::ivec2 at = { x, z };
			if (canLoadColumn(at))
				loadColumn(at);
		}

	// Unload
	for (auto &c : m_columns)
	{
		glm::fvec2 playPosChunks = glm::ivec2(playerPos.x / CHUNK_SIZE, playerPos.z / CHUNK_SIZE);
		glm::fvec2 colPos = (c.first);
		if (glm::distance(colPos, playPosChunks) > 7)
		{
			if (canUnloadColumn(c.first))
				unloadColumn(c.first);
		}
	}

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

    double time = glfwGetTime();

    auto current = m_toLoadColumns.front();
    auto newPair = std::make_pair(current, std::make_unique<ChunkColumn>());
    // Add and get last
    m_columns.push_back(std::move(newPair));
    auto &last = m_columns.back();

    last.second->generateTerrain(last.first);

    setNeighboors(&last, true);
    rebuildNeighboors(&last);
    last.second->generateMeshes(current);

    m_toLoadColumns.pop_front();
    LOG_TRACE("Fully loaded column {}, {} ({}ms)", current.x, current.y, (glfwGetTime() - time) * 1000);
    return true;
}

bool World::poolUnload()
{
	// Chunks should NEVER be unloaded by an other thread than the main thread
	// This is because ChunkMeshes require a valid OpenGL context to work properly
	// and not providing one would cause a silent memory leak

	if (glfwGetCurrentContext() == NULL)
	{
		LOG_CRITICAL("Memory leak: calling {} function from a thread with no OpenGL context", __FUNCTION__);
		throw RuntimeException("Calling unload function on thread that does not own the OpenGL context");
	} // Rudimentary safety guard to check that

    if (m_toUnloadColumns.empty())
        return false;

	glm::ivec2 at = m_toUnloadColumns.front();

	auto it = std::find_if(
		m_columns.begin(),
		m_columns.end(),
		[&](const WorldColumn &current) {
		return current.first == at;
	});

	if (it == m_columns.end())
		throw RuntimeException("Column at (" + std::to_string(at.x) + ", " + std::to_string(at.y) + ") is not in list");

	m_columns.erase(it);
	m_toUnloadColumns.pop_front();

    LOG_TRACE("Fully unloaded column {}, {}", at.x, at.y);
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
