#pragma once

#include "pch.h"

#include <glad/glad.h>

namespace tk
{
class Shader
{
  public:
    Shader(const std::string &vertPath, const std::string &fragPath);
    ~Shader();

  private:
    void createShader(GLenum shaderType);
    void deleteShader(GLenum shaderType);
    void createProgram();
    void deleteProgram();

  private:
    std::string m_vertPath;
    std::string m_fragPath;

    GLuint m_vertexShader, m_fragmentShader, m_shaderProgram;

  public:
    Shader(const Shader &) = delete;
    void operator=(const Shader &) = delete;
};

} // namespace tk
