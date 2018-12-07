#pragma once

#include "pch.h"

namespace tk
{
class Chunk
{
  public:
    Chunk();
    Chunk(const Block &filledWith);
    ~Chunk();

    void setBlock(const glm::uvec3 &at, const Block &block);
    const Block *getBlock(const glm::uvec3 &at) const;

    void fillWith(const Block &block) noexcept;
    void fillLayerWith(unsigned layer, const Block &block);
    void fillColumnWith(const glm::uvec2 &column, unsigned from, unsigned to, const Block &with);
    void fillColumnWith(const glm::uvec2 &column, const Block &with);

  private:
    inline unsigned toArrayCoordinates(const glm::uvec3 &from) const noexcept { return from.x + CHUNK_SIZE * from.y + CHUNK_SIZE * CHUNK_SIZE * from.z; }
    inline bool isPositionValid(const glm::uvec3 &pos) const noexcept { return pos.x < CHUNK_SIZE && pos.y < CHUNK_SIZE && pos.z < CHUNK_SIZE; }

  private:
    std::array<const Block *, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> m_blocks;
    bool m_isAir;

  private:
};
} // namespace tk