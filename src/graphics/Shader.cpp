#include "Shader.h"
#include "../utils/Exceptions.h"
#include "../utils/glutils.h"

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

void Shader::reload() noexcept
{
    LOG_DEBUG("Reloading shader \"{}, {}\"...", m_vertPath, m_fragPath);
    deleteProgram();
    load();
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
    //flushUniformCache();
}

} // namespace tk
