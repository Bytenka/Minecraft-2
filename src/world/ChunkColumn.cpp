#include "pch.h"
#include "ChunkColumn.h"

#include "TerrainGenerator.h"

#include <exception>

namespace tk
{
std::unique_ptr<Chunk> ChunkColumn::s_borderChunk(nullptr);

ChunkColumn::ChunkColumn()
{
    if (!s_borderChunk)
        s_borderChunk = std::make_unique<Chunk>(Blocks::_air); // _air specified for clarity

    adjacentColumns.fill(nullptr);
}

ChunkColumn::~ChunkColumn()
{
}

// public:

void ChunkColumn::setBlock(const glm::uvec3 &at, const Block &block)
{
    if (at.y > CHUNK_SIZE * CHUNK_COL_HEIGHT)
        throw std::out_of_range("Height " + std::to_string(at.y) + " for block is out of range");

    m_chunks[at.y / CHUNK_COL_HEIGHT].setBlock({at.x, at.y % 16, at.z}, block);
}

const Block *ChunkColumn::getBlock(const glm::uvec3 &at) const
{
    if (at.y > CHUNK_SIZE * CHUNK_COL_HEIGHT)
        throw std::out_of_range("Height " + std::to_string(at.y) + " for block is out of range");

    return m_chunks[at.y / CHUNK_COL_HEIGHT].getBlock({at.x, at.y % 16, at.z});
}

Chunk *ChunkColumn::getChunk(unsigned atIndex)
{
    if (atIndex >= CHUNK_COL_HEIGHT)
        throw std::out_of_range("Index " + std::to_string(atIndex) + " for chunk is out of range");

    return &m_chunks[atIndex];
}

std::vector<RenderData> ChunkColumn::getRenderData() noexcept
{
    std::vector<RenderData> toReturn;

    for (auto &c : m_chunks)
    {
        toReturn.push_back(c.getRenderData());
    }

    return toReturn;
}

void ChunkColumn::fillColumn(const glm::uvec2 &column, const Block &block)
{
    fillColumn(column, 0, CHUNK_COL_HEIGHT * CHUNK_SIZE - 1, block);
}

void ChunkColumn::fillColumn(const glm::uvec2 &column, unsigned from, unsigned to, const Block &block)
{
    if (column.x >= CHUNK_SIZE || column.y >= CHUNK_SIZE)
        throw std::out_of_range("Position (" + std::to_string(column.x) + ", " + std::to_string(column.y) + ") is out of range");

    if (from >= CHUNK_COL_HEIGHT * CHUNK_SIZE)
        throw std::out_of_range("Starting point " + std::to_string(from) + " is out of range");

    if (to >= CHUNK_COL_HEIGHT * CHUNK_SIZE)
        throw std::out_of_range("Ending point " + std::to_string(to) + " is out of range");

    for (; from <= to; from++)
    {
        Chunk &currentChunk = m_chunks[from / CHUNK_COL_HEIGHT];
        currentChunk.setBlock({column.x, from % CHUNK_SIZE, column.y}, block);
    }
}

void ChunkColumn::generateTerrain(const glm::ivec2 &forPosition) noexcept
{
    TerrainGenerator &tg = TerrainGenerator::getInstance();

    for (int x = 0; x < CHUNK_SIZE; x++)
        for (int z = 0; z < CHUNK_SIZE; z++)
        {
            glm::ivec2 localPos = {x, z};
            fillColumn(localPos, 0, tg.getHeight(forPosition * CHUNK_SIZE + localPos) / 2 + 127, Blocks::grass);
        }
}

void ChunkColumn::generateMeshesFor(unsigned chunkIndex, const glm::ivec2 &columnPos)
{
    if (chunkIndex >= CHUNK_COL_HEIGHT)
        throw std::out_of_range("Index " + std::to_string(chunkIndex) + " for chunk is out of range");

    std::array<const Chunk *, 6> adjacentChunks;

    adjacentChunks[0] = chunkIndex >= CHUNK_COL_HEIGHT - 1 ? s_borderChunk.get() : &m_chunks[chunkIndex + 1]; // Top
    adjacentChunks[1] = chunkIndex == 0 ? s_borderChunk.get() : &m_chunks[chunkIndex - 1];                    // Bottom
    adjacentChunks[2] = adjacentColumns[0] == nullptr ? nullptr : adjacentColumns[0]->getChunk(chunkIndex);   // Left
    adjacentChunks[3] = adjacentColumns[1] == nullptr ? nullptr : adjacentColumns[1]->getChunk(chunkIndex);   // Right
    adjacentChunks[4] = adjacentColumns[2] == nullptr ? nullptr : adjacentColumns[2]->getChunk(chunkIndex);   // Front
    adjacentChunks[5] = adjacentColumns[3] == nullptr ? nullptr : adjacentColumns[3]->getChunk(chunkIndex);   // Back

    m_chunks[chunkIndex].generateMesh({columnPos.x, chunkIndex, columnPos.y}, adjacentChunks);
}

void ChunkColumn::generateMeshes(const glm::ivec2 &columnPos)
{
    for (int i = m_chunks.size() - 1; i >= 0; i--)
        generateMeshesFor(i, columnPos);
}

// private:

} // namespace tk
