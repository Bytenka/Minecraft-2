#include "pch.h"
#include "ChunkMesh.h"

#include "../graphics/TextureAtlas.h"
#include "../utils/Exceptions.h"
#include "Constants.h"

#include <glm/glm.hpp>

namespace tk
{
ChunkMesh::ChunkMesh()
{
}

ChunkMesh::~ChunkMesh()
{
}

// public:
void ChunkMesh::addFace(const Block &block, BlockFace face, const glm::ivec3 &chunkPos, const glm::uvec3 &blockPos) noexcept
{
    try
    {
        auto text = tk::TextureAtlas::getInstance().getTextureCoordinates(block.textures[face]);
        m_textCoords.insert(m_textCoords.end(), text.begin(), text.end());
    }
    catch (RuntimeException &e)
    {
        LOG_ERROR("Unable to apply texture to face: {}", e.what());
    }

    // 4 vertices to add per face, 3 coords per vertex
    for (unsigned i = 0; i < 4; i++)
    {
        unsigned index = 3 * i;
        m_vertCoords.push_back(blockFaceCoordinates[face][index + 0] + chunkPos.x * CHUNK_SIZE + blockPos.x);
        m_vertCoords.push_back(blockFaceCoordinates[face][index + 1] + chunkPos.y * CHUNK_SIZE + blockPos.y);
        m_vertCoords.push_back(blockFaceCoordinates[face][index + 2] + chunkPos.z * CHUNK_SIZE + blockPos.z);
    }

    m_indices.insert(
        m_indices.end(),
        {m_indexOffset,
         m_indexOffset + 1,
         m_indexOffset + 2,

         m_indexOffset,
         m_indexOffset + 2,
         m_indexOffset + 3});

    m_indexOffset += 4;
    m_isUsable = false;
}

GLuint ChunkMesh::getVAO()
{
    if (!m_isUsable)
        push();
    return m_vao;
}

void ChunkMesh::clear() noexcept
{
    m_vertCoords.clear();
    m_textCoords.clear();
    m_indices.clear();

    m_indexOffset = 0;

    clearGL();
}

// private:

void ChunkMesh::push()
{
    clearGL();

    GLuint vao, vboVert, vboText, ebo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vboVert);
    glGenBuffers(1, &vboText);
    glGenBuffers(1, &ebo);

    if (!vao || !vboVert || !vboText || !ebo)
        throw RuntimeException("OpenGL call to create buffers failed");

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vboVert);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_vertCoords.size(), m_vertCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vboText);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_textCoords.size(), m_textCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, (void *)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat) * m_indices.size(), m_indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    m_vao = vao;
    m_vboVert = vboVert;
    m_vboText = vboText;
    m_ebo = ebo;

    m_isUsable = true;
}

void ChunkMesh::clearGL() noexcept
{
    if (m_vboVert != 0)
    {
        glDeleteBuffers(1, &m_vboVert);
        m_vboVert = 0;
    }

    if (m_vboText != 0)
    {
        glDeleteBuffers(1, &m_vboText);
        m_vboText = 0;
    }

    if (m_ebo != 0)
    {
        glDeleteBuffers(1, &m_ebo);
        m_ebo = 0;
    }

    if (m_vao != 0)
    {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }

    m_isUsable = false;
}
} // namespace tk