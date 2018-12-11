#pragma once

#include "Block.h"
#include "Constants.h"
#include "ChunkMesh.h"

#include <glm/glm.hpp>

#include <array>

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

  void generateMesh(const glm::ivec3 &forPosition, const std::array<Chunk *, 6> &neighboors);
  inline ChunkMesh &getMesh() noexcept { return m_mesh; }

private:
  inline unsigned toArrayCoordinates(const glm::uvec3 &from) const noexcept { return from.x + CHUNK_SIZE * from.y + CHUNK_SIZE * CHUNK_SIZE * from.z; }
  inline bool isPositionValid(const glm::uvec3 &pos) const noexcept { return pos.x < CHUNK_SIZE && pos.y < CHUNK_SIZE && pos.z < CHUNK_SIZE; }

  const Block *getBlockNeighboors(const glm::ivec3 &at, const std::array<Chunk *, 6> &neighboors) const;

private:
  std::array<const Block *, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> m_blocks;
  bool m_isAir;

  ChunkMesh m_mesh;

private:
};
} // namespace tk