/*
    For now, this is just using a single pregenerated atlas and ccordinates are hardcoded
    Should use some system to pack up textures at load
*/

#pragma once

#include "Image.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <memory>
#include <map>
#include <array>

namespace tk
{
struct TextureData
{
    TextureData(const glm::uvec2 &top_left_corner, unsigned width, unsigned height)
        : top_left_corner(top_left_corner), width(width), height(height) {}
    glm::uvec2 top_left_corner;
    unsigned width;
    unsigned height;
};

class TextureAtlas
{
  public:
    inline static TextureAtlas &getInstance()
    {
        static TextureAtlas instance;
        return instance;
    }

    void init(const std::string &file);
    void reload();
    void release() noexcept;
    std::array<GLfloat, 8> getTextureCoordinates(const std::string &textureName);

  private:
    TextureAtlas();
    ~TextureAtlas();

    void setupCoordinates() noexcept;
    glm::dvec2 normalizeCoordinates(const glm::uvec2 &coordinates) const noexcept;
    std::array<GLfloat, 8> toGLCoordinates(const TextureData &data) const noexcept;

    void pushGL();
    void deleteGL() noexcept;

  private:
    std::string m_atlasPath;
    std::unique_ptr<Image> m_atlasImg;
    std::map<std::string, TextureData> m_atlas;

    bool m_isLoaded = false;
    std::string m_defaultTextureName = "default_texture";
    GLuint m_atlasID = 0;

  public:
    TextureAtlas(const TextureAtlas &) = delete;
    void operator=(const TextureAtlas &) = delete;
};
} // namespace tk

// Singleton pattern from https://stackoverflow.com/questions/1008019/c-singleton-design-pattern/1008289#1008289
