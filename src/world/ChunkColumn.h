#pragma once

#include "Chunk.h"
#include "ChunkMesh.h"

#include <glm/glm.hpp>

#include <array>
#include <memory>

namespace tk
{
class ChunkColumn
{
public:
  ChunkColumn();
  ~ChunkColumn();

  void setBlock(const glm::uvec3 &at, const Block &block);
  const Block *getBlock(const glm::uvec3 &at) const;
  Chunk *getChunk(unsigned atIndex);
  std::vector<RenderData> getRenderData() noexcept;

  void generateMeshesFor(unsigned chunkIndex, const glm::ivec2 &columnPos);
  void generateMeshes(const glm::ivec2 &columnPos);

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
