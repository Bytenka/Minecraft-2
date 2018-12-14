#pragma once

#include "ChunkColumn.h"
#include "ChunkMesh.h"

#include <glm/glm.hpp>

#include <vector>
#include <utility>
#include <memory>
#include <deque>
#include <mutex>

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
  const std::vector<RenderData> &getRenderData() noexcept;

  static void updateLoop(World &world, const glm::dvec3 &playerPos, bool &shouldStop);

private:
  void update(const glm::dvec3 &playerPos);

  bool poolLoad();   // Returns true when something has been loaded, false otherwise
  bool poolUnload(); // Returns true when something has been unloaded, false otherwise
  std::vector<WorldColumn>::iterator findColumn(const glm::ivec2 &at);
  void setNeighboors(WorldColumn *ofColumn, bool presenceStatus) noexcept;

  void rebuildNeighboors(WorldColumn *ofColumn);

private:
  std::vector<WorldColumn> m_columns;
  std::deque<glm::ivec2> m_toLoadColumns;
  std::deque<glm::ivec2> m_toUnloadColumns;
  std::vector<RenderData> m_drawCache;
  std::mutex m_mainMutex;

public:
  World(const World &) = delete;
  void operator=(const World &) = delete;
};
} // namespace tk
