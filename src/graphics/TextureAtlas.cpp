#include "pch.h"
#include "TextureAtlas.h"

#include "../utils/Log.h"
#include "../utils/Exceptions.h"

namespace tk
{
TextureAtlas::TextureAtlas()
{
    LOG_INFO("Texture atlas initialized");
}

TextureAtlas::~TextureAtlas()
{
}

// public:

void TextureAtlas::init(const std::string &file)
{
    if (m_isLoaded)
        throw RuntimeException("Texture atlas is already loaded");

    m_atlasImg = std::make_unique<Image>(file);

    setupCoordinates();
    pushGL();
    m_atlasPath = file;
    m_isLoaded = true;
}

void TextureAtlas::reload()
{
    release();
    init(m_atlasPath);
}

void TextureAtlas::release() noexcept
{
    m_atlasImg.release();
    m_atlas.clear();

    deleteGL();
    m_isLoaded = false;
}

std::array<GLfloat, 8> TextureAtlas::getTextureCoordinates(const std::string &textureName)
{
    bool isDefault = textureName == m_defaultTextureName;

    if (!m_isLoaded)
        throw RuntimeException("Texture atlas is not loaded");

    auto it = m_atlas.find(textureName);

    if (it == m_atlas.end())
    {
        if (isDefault)
            throw RuntimeException("Default texture not found");
        else
            return getTextureCoordinates(m_defaultTextureName);
    }

    return toGLCoordinates(it->second);
}

// private:

void TextureAtlas::setupCoordinates() noexcept
{
    std::pair<std::string, TextureData> pairs[12] = {
        std::make_pair("tnt_side", TextureData(glm::uvec2(128, 0), 16, 16)),
        std::make_pair("tnt_top", TextureData(glm::uvec2(144, 0), 16, 16)),
        std::make_pair("tnt_bottom", TextureData(glm::uvec2(160, 0), 16, 16)),
        std::make_pair("grass_top", TextureData(glm::uvec2(192, 192), 16, 16)),
        std::make_pair("dirt", TextureData(glm::uvec2(32, 0), 16, 16)),
        std::make_pair("grass_side", TextureData(glm::uvec2(48, 0), 16, 16)),
        std::make_pair("diamond_ore", TextureData(glm::uvec2(32, 48), 16, 16)),
        std::make_pair("obsidian", TextureData(glm::uvec2(80, 32), 16, 16)),
        std::make_pair("cobblestone", TextureData(glm::uvec2(0, 16), 16, 16)),
        std::make_pair("bedrock", TextureData(glm::uvec2(16, 16), 16, 16)),
        std::make_pair("stone", TextureData(glm::uvec2(16, 0), 16, 16)),

        std::make_pair(m_defaultTextureName, TextureData(glm::uvec2(208, 240), 16, 16))};
    // This last one MUST exist and MUST be valid (exception if not)

    // @TODO This is sketchy
    unsigned nb = sizeof(pairs) / sizeof(std::pair<std::string, TextureData>);
    for (unsigned i = 0; i < nb; i++)
    {
        m_atlas.insert(pairs[i]);
    }
}

glm::dvec2 TextureAtlas::normalizeCoordinates(const glm::uvec2 &coordinates) const noexcept
{
    glm::dvec2 toReturn;
    toReturn.x = coordinates.x / (double)m_atlasImg->getWidth();
    toReturn.y = coordinates.y / (double)m_atlasImg->getHeight();

    return toReturn;
}

std::array<GLfloat, 8> TextureAtlas::toGLCoordinates(const TextureData &data) const noexcept
{
    glm::fvec2 topLeft = (glm::fvec2)normalizeCoordinates(data.top_left_corner);
    glm::fvec2 topRight = (glm::fvec2)normalizeCoordinates({data.top_left_corner.x + data.width, data.top_left_corner.y});
    glm::fvec2 bottomLeft = (glm::fvec2)normalizeCoordinates({data.top_left_corner.x, data.top_left_corner.y + data.height});
    glm::fvec2 bottomRight = (glm::fvec2)normalizeCoordinates({data.top_left_corner.x + data.width, data.top_left_corner.y + data.width});

    std::array<GLfloat, 8> toReturn = {
        bottomLeft.x, bottomLeft.y,
        bottomRight.x, bottomRight.y,
        topRight.x, topRight.y,
        topLeft.x, topLeft.y};

    return toReturn;
}

void TextureAtlas::pushGL()
{
    GLuint id;
    glGenTextures(1, &id);

    if (id == 0)
        throw RuntimeException("OpenGL call to create texture failed");

    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        m_atlasImg->getWidth(),
        m_atlasImg->getHeight(),
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        m_atlasImg->getData());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenerateMipmap(GL_TEXTURE_2D);

    m_atlasID = id;
}

void TextureAtlas::deleteGL() noexcept
{
    if (m_atlasID != 0)
    {
        glDeleteTextures(1, &m_atlasID);
        m_atlasID = 0;
    }
}

} // namespace tk
