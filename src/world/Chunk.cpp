#include "pch.h"
#include "Chunk.h"

#include "../utils/Log.h"

#include <string>
#include <exception>

#include <GLFW/glfw3.h>
#ifdef DEBUG
#define BLOCK_IF_VOID _opaque
#else
#define BLOCK_IF_VOID _opaque
#endif

namespace tk
{
Chunk::Chunk()
{
    fillWith(Blocks::_air);
}
Chunk::Chunk(const Block &filledWith)
{
    fillWith(filledWith);
}

Chunk::~Chunk()
{
}

// public:

void Chunk::setBlock(const glm::uvec3 &at, const Block &block)
{
    if (!isPositionValid(at))
        throw std::out_of_range("Block position (" + std::to_string(at.x) + ", " + std::to_string(at.y) + ", " + std::to_string(at.z) + ") is out of range");

    m_blocks[toArrayCoordinates(at)] = &block;
}

const Block *Chunk::getBlock(const glm::uvec3 &at) const
{
    if (!isPositionValid(at))
        throw std::out_of_range("Block at (" + std::to_string(at.x) + ", " + std::to_string(at.y) + ", " + std::to_string(at.z) + ") is out of range");

    return m_blocks[toArrayCoordinates(at)];
}

void Chunk::fillWith(const Block &block) noexcept
{
    m_blocks.fill(&block);
    m_isAir = &block == &Blocks::_air;
}

void Chunk::fillLayerWith(unsigned layer, const Block &block)
{
    if (layer >= CHUNK_SIZE)
        throw std::out_of_range("Layer " + std::to_string(layer) + " is out of range");

    for (unsigned x = 0; x < CHUNK_SIZE; x++)
        for (unsigned z = 0; z < CHUNK_SIZE; z++)
        {
            m_blocks[toArrayCoordinates({x, layer, z})] = &block;
        }

    if (&block != &Blocks::_air)
        m_isAir = false;
}

void Chunk::fillColumnWith(const glm::uvec2 &column, unsigned from, unsigned to, const Block &with)
{
    if (!isPositionValid({column.x, 0, column.y}))
        throw std::out_of_range("Column (" + std::to_string(column.x) + ", " + std::to_string(column.y) + ") is out of range");

    if (from >= CHUNK_SIZE)
        throw std::out_of_range("Start position " + std::to_string(from) + " is out of range");

    if (to >= CHUNK_SIZE)
        throw std::out_of_range("End position " + std::to_string(to) + " is out of range");

    for (; from <= to; from++)
        m_blocks[toArrayCoordinates({column.x, from, column.y})] = &with;

    if (&with != &Blocks::_air)
        m_isAir = false;
}

void Chunk::fillColumnWith(const glm::uvec2 &column, const Block &with)
{
    fillColumnWith(column, 0, CHUNK_SIZE - 1, with);
}

void Chunk::generateMesh(const glm::ivec3 &forPosition, const std::array<const Chunk *, 6> &neighboors)
{
    m_mesh.clear();

    for (unsigned x = 0; x < CHUNK_SIZE; x++)
        for (unsigned y = 0; y < CHUNK_SIZE; y++)
            for (unsigned z = 0; z < CHUNK_SIZE; z++)
            {
                const Block *currentBlock = m_blocks[toArrayCoordinates({x, y, z})];

                if (currentBlock == &Blocks::_air || currentBlock == &Blocks::_opaque)
                    continue;

                if (!getBlockNeighboors({x, y + 1, z}, neighboors)->data.isOpaque)
                    m_mesh.addFace(*currentBlock, BlockFace::TOP, forPosition, {x, y, z});

                if (!getBlockNeighboors({x, y - 1, z}, neighboors)->data.isOpaque)
                    m_mesh.addFace(*currentBlock, BlockFace::BOTTOM, forPosition, {x, y, z});

                if (!getBlockNeighboors({x - 1, y, z}, neighboors)->data.isOpaque)
                    m_mesh.addFace(*currentBlock, BlockFace::LEFT, forPosition, {x, y, z});

                if (!getBlockNeighboors({x + 1, y, z}, neighboors)->data.isOpaque)
                    m_mesh.addFace(*currentBlock, BlockFace::RIGHT, forPosition, {x, y, z});

                if (!getBlockNeighboors({x, y, z + 1}, neighboors)->data.isOpaque)
                    m_mesh.addFace(*currentBlock, BlockFace::FRONT, forPosition, {x, y, z});

                if (!getBlockNeighboors({x, y, z - 1}, neighboors)->data.isOpaque)
                    m_mesh.addFace(*currentBlock, BlockFace::BACK, forPosition, {x, y, z});
            }
}

RenderData Chunk::getRenderData() noexcept
{
    RenderData toReturn;
    toReturn.vao = m_mesh.getVAO();
    toReturn.nbVertices = m_mesh.getVerticesCount();
    return toReturn;
}

// private:

const Block *Chunk::getBlockNeighboors(const glm::ivec3 &at, const std::array<const Chunk *, 6> &neighboors) const
{
    // Cast to unsigned. Error will be catched in getBlock calls
    glm::uvec3 newPos = (at + CHUNK_SIZE) % CHUNK_SIZE;

    // ------------------------------------------- //
    if (at.y >= CHUNK_SIZE) // Top chunk
        return neighboors[0] == nullptr ? &Blocks::BLOCK_IF_VOID : neighboors[0]->getBlock(newPos);

    if (at.y < 0) // Bottom chunk
        return neighboors[1] == nullptr ? &Blocks::BLOCK_IF_VOID : neighboors[1]->getBlock(newPos);
    // Technically, those two above are never null //

    if (at.x < 0) // Left chunk
        return neighboors[2] == nullptr ? &Blocks::BLOCK_IF_VOID : neighboors[2]->getBlock(newPos);

    if (at.x >= CHUNK_SIZE) // Right chunk
        return neighboors[3] == nullptr ? &Blocks::BLOCK_IF_VOID : neighboors[3]->getBlock(newPos);

    if (at.z >= CHUNK_SIZE) // Front chunk
        return neighboors[4] == nullptr ? &Blocks::BLOCK_IF_VOID : neighboors[4]->getBlock(newPos);

    if (at.z < 0) // Back chunk
        return neighboors[5] == nullptr ? &Blocks::BLOCK_IF_VOID : neighboors[5]->getBlock(newPos);

    return getBlock(at);
}
} // namespace tk