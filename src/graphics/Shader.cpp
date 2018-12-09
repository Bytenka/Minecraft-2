#include "pch.h"
#include "Shader.h"

#include "../utils/Exceptions.h"
#include "../utils/glutils.h"

#include <glm/gtc/type_ptr.hpp>

#include <exception>

namespace tk
{

Shader::Shader(const std::string &vertPath, const std::string &fragPath)
    : m_vertPath(vertPath), m_fragPath(fragPath)
{
    load();
}

Shader::~Shader()
{
    deleteProgram();
}

// public:

void Shader::reload()
{
    LOG_DEBUG("Reloading shader \"{}, {}\"...", m_vertPath, m_fragPath);
    deleteProgram();
    load();
}

void Shader::setUniform1i(const std::string &uniformName, int i)
{
    enable();
    glUniform1i(getUniformLocation(uniformName), i);
}

void Shader::setUniform1f(const std::string &uniformName, float f)
{
    enable();
    glUniform1f(getUniformLocation(uniformName), f);
}

void Shader::setUniform2f(const std::string &uniformName, float x, float y)
{
    enable();
    glUniform2f(getUniformLocation(uniformName), x, y);
}

void Shader::setUniform3f(const std::string &uniformName, float x, float y, float z)
{
    enable();
    glUniform3f(getUniformLocation(uniformName), x, y, z);
}

void Shader::setUniform4f(const std::string &uniformName, float x, float y, float z, float w)
{
    enable();
    glUniform4f(getUniformLocation(uniformName), x, y, z, w);
}

void Shader::setUniformMatrix4fv(const std::string &uniformName, const glm::mat4 &transform)
{
    enable();
    glUniformMatrix4fv(getUniformLocation(uniformName), 1, GL_FALSE, glm::value_ptr(transform));
}

// private:

void Shader::load()
{
    createShader(GL_VERTEX_SHADER);
    createShader(GL_FRAGMENT_SHADER);

    createProgram();

    deleteShader(GL_VERTEX_SHADER);
    deleteShader(GL_FRAGMENT_SHADER);

    LOG_INFO("Loaded shader \"{}, {}\"", m_vertPath, m_fragPath);
}

void Shader::createShader(GLenum shaderType)
{
    try
    {
        GLuint *toModify = nullptr;
        std::string *toModifyPath = nullptr;
        switch (shaderType)
        {
        case GL_VERTEX_SHADER:
            toModify = &m_vertexShader;
            toModifyPath = &m_vertPath;
            break;
        case GL_FRAGMENT_SHADER:
            toModify = &m_fragmentShader;
            toModifyPath = &m_fragPath;
            break;
        default:
            throw std::invalid_argument("Shader type unsupported or incorrect");
            break;
        }

        std::string shaSourceStr(read_file(toModifyPath->c_str()));
        const char *shaSource = shaSourceStr.c_str();

        GLuint sha = glCreateShader(shaderType);
        if (sha == 0)
            throw RuntimeException("OpenGL call to create shader failed");

        glShaderSource(sha, 1, &shaSource, NULL);
        glCompileShader(sha);

        // Error checking
        int success;
        glGetShaderiv(sha, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(sha, 512, NULL, infoLog);
            throw RuntimeException("Compilation error: " + std::string(infoLog));
        }

        *toModify = sha;
    }
    catch (RuntimeException &e)
    {
        LOG_ERROR("Could not create shader: {}", e.what());
        throw;
    }
}

void Shader::deleteShader(GLenum shaderType)
{
    switch (shaderType)
    {
    case GL_VERTEX_SHADER:
        glDeleteShader(m_vertexShader);
        break;
    case GL_FRAGMENT_SHADER:
        glDeleteShader(m_fragmentShader);
        break;
    default:
        throw std::invalid_argument("Shader type unsupported or incorrect");
        break;
    }
}

void Shader::createProgram()
{
    GLuint prog = glCreateProgram();
    if (prog == 0)
        throw RuntimeException("OpenGL call to create program shader failed");

    glAttachShader(prog, m_vertexShader);
    glAttachShader(prog, m_fragmentShader);
    glLinkProgram(prog);

    try
    {
        // Error checking
        int success;
        glGetProgramiv(prog, GL_LINK_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetProgramInfoLog(prog, 512, NULL, infoLog);
            throw RuntimeException("Linking error: " + std::string(infoLog));
        }
    }
    catch (RuntimeException &e)
    {
        LOG_ERROR("Could not create program shader: {}", e.what());
        throw;
    }

    m_shaderProgram = prog;
}

void Shader::deleteProgram() noexcept
{
    glDeleteProgram(m_shaderProgram);
    m_uniformCache.clear();
}

GLint Shader::getUniformLocation(const std::string &uniformName)
{
    auto it = std::find_if(
        m_uniformCache.begin(),
        m_uniformCache.end(),
        [&](const std::pair<std::string, GLuint> &current) {
            return (current.first) == uniformName;
        });

    if (it != m_uniformCache.end())
        return it->second;

    // If not in cache, add it
    GLint newValue = glGetUniformLocation(m_shaderProgram, uniformName.c_str());

    if (newValue == -1)
        throw std::invalid_argument("Uniform \"" + uniformName + "\" does not exist or is invalid");

    m_uniformCache.push_back(std::make_pair(uniformName, newValue));

    return newValue;
}

} // namespace tk
