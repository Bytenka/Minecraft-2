#include "Chunk.h"

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

// private:
} // namespace tk