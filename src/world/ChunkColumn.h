#pragma once

#include "Chunk.h"

#include <array>
#include <memory>

namespace tk
{
class ChunkColumn
{
  public:
    ChunkColumn(const glm::ivec2 &position);
    ~ChunkColumn();

    void setBlock(const glm::uvec3 &at, const Block &block);
    const Block *getBlock(const glm::uvec3 &at) const;
    Chunk *getChunk(unsigned atIndex);

    void generateMeshesFor(unsigned chunkIndex);
    void generateMeshes();

    inline glm::ivec2 getPosition() const noexcept { return m_position; }

  public:
    // [0]:Left, [1]:Right, [2]:Front, [3]:Back
    std::array<ChunkColumn *, 4> adjacentColumns; // Caching

  private:
  private:
    glm::ivec2 m_position;
    std::array<Chunk, CHUNK_COL_HEIGHT> m_chunks;
    static std::unique_ptr<Chunk> s_borderChunk; // Used as neighboor for top and bottom chunks

  public:
    ChunkColumn(const ChunkColumn &) = delete;
    void operator=(const ChunkColumn &) = delete;
};
} // namespace tk
