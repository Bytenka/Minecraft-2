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
  void reload();

  void setUniform1i(const std::string &uniformName, int i);
  void setUniform1f(const std::string &uniformName, float f);
  void setUniform2f(const std::string &uniformName, float x, float y);
  void setUniform3f(const std::string &uniformName, float x, float y, float z);
  void setUniform4f(const std::string &uniformName, float x, float y, float z, float w);
  void setUniformMatrix4fv(const std::string &uniformName, const glm::mat4 &transform);

private:
  void load();
  void createShader(GLenum shaderType);
  void deleteShader(GLenum shaderType);
  void createProgram();
  void deleteProgram() noexcept;

  GLint getUniformLocation(const std::string &uniformName);

private:
  std::string m_vertPath;
  std::string m_fragPath;

  GLuint m_vertexShader, m_fragmentShader, m_shaderProgram;
  std::vector<std::pair<std::string, GLint>> m_uniformCache;

public:
  Shader(const Shader &) = delete;
  void operator=(const Shader &) = delete;
};

} // namespace tk
