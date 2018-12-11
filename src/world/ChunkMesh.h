#pragma once

#include "Block.h"

#include <glad/glad.h>

#include <vector>

namespace tk
{
class ChunkMesh
{
public:
  ChunkMesh();
  ~ChunkMesh();

  void addFace(const Block &block, BlockFace face, const glm::ivec3 &chunkPos, const glm::uvec3 &blockPos) noexcept;
  GLuint getVAO();
  void clear() noexcept;

  inline unsigned getVerticesCount() const noexcept { return m_indices.size(); }

private:
  void push();
  void clearGL() noexcept;

private:
  std::vector<GLfloat> m_vertCoords;
  std::vector<GLfloat> m_textCoords;
  std::vector<GLuint> m_indices;

  GLuint m_vao = 0, m_vboVert = 0, m_vboText = 0, m_ebo = 0;

  unsigned m_indexOffset = 0;
  bool m_isUsable = false;
};
} // namespace tk