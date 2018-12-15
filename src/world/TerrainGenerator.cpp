#include "pch.h"
#include "TerrainGenerator.h"

#include "Constants.h"
#include "../utils/Log.h"

#define SMOOTHNESS 200

namespace tk
{
TerrainGenerator::TerrainGenerator(unsigned seed)
{
    m_pn.reseed(seed);
    LOG_INFO("Initialized terrain generator");
}

TerrainGenerator::~TerrainGenerator()
{
}

unsigned TerrainGenerator::getHeight(const glm::ivec2 &at) noexcept
{
    float value = m_pn.octaveNoise0_1((float)at.x / SMOOTHNESS, (float)at.y / SMOOTHNESS, 3);
    return (unsigned)(value * CHUNK_COL_HEIGHT * CHUNK_SIZE);
}
} // namespace tk
