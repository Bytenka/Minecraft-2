#pragma once

#include "ChunkColumn.h"
#include "ChunkMesh.h"

#include <glm/glm.hpp>

#include <vector>
#include <utility>
#include <memory>

namespace tk
{
typedef std::pair<glm::ivec2, std::unique_ptr<ChunkColumn>> WorldColumn;
class World
{
public:
  World();
  ~World();

  void loadColumn(const glm::ivec2 &at);
  void unloadColumn(const glm::ivec2 &at);

  ChunkColumn *getColumn(const glm::ivec2 &at);
  std::vector<RenderData> getRenderData() noexcept;

private:
  std::vector<WorldColumn>::iterator findColumn(const glm::ivec2 &at);
  void setNeighboors(WorldColumn *ofColumn, bool presenceStatus) noexcept;

private:
  std::vector<WorldColumn> m_columns;

public:
  World(const World &) = delete;
  void operator=(const World &) = delete;
};
} // namespace tk
