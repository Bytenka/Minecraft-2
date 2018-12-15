#pragma once

#include <glm/glm.hpp>
#include "../utils/PerlinNoise.h"

namespace tk
{
class TerrainGenerator
{
  public:
    inline static TerrainGenerator &getInstance() noexcept
    {
        static TerrainGenerator instance;
        return instance;
    }
    unsigned getHeight(const glm::ivec2 &at) noexcept;

  private:
    PerlinNoise m_pn;

  private:
    TerrainGenerator(unsigned seed = 12345);
    ~TerrainGenerator();

  public:
    TerrainGenerator(const TerrainGenerator &) = delete;
    void operator=(const TerrainGenerator &) = delete;
};
} // namespace tk
