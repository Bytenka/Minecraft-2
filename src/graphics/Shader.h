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

  inline void enable() const noexcept { glUseProgram(m_shaderProgram); }
  inline void disable() const noexcept { glUseProgram(0); }
  void reload() noexcept;

private:
  void load();
  void createShader(GLenum shaderType);
  void deleteShader(GLenum shaderType);
  void createProgram();
  void deleteProgram() noexcept;

private:
  std::string m_vertPath;
  std::string m_fragPath;

  GLuint m_vertexShader, m_fragmentShader, m_shaderProgram;

public:
  Shader(const Shader &) = delete;
  void operator=(const Shader &) = delete;
};

} // namespace tk
