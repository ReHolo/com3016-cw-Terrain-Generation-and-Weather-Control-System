#include "ParticleShader.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

ParticleShader::ParticleShader(const std::string& vertexSrc, const std::string& fragmentSrc)
    : m_programID(0), m_projLoc(-1), m_viewLoc(-1)
{
    // 1. ���붥�� & Ƭ����ɫ��
    unsigned int vs = compileShader(vertexSrc.c_str(), GL_VERTEX_SHADER);
    unsigned int fs = compileShader(fragmentSrc.c_str(), GL_FRAGMENT_SHADER);

    // 2. ���ӳ�һ����������ɫ������
    m_programID = linkProgram(vs, fs);

    // 3. ɾ��������Ҫ���м���ɫ������
    glDeleteShader(vs);
    glDeleteShader(fs);

    // 4. ��ȡ Uniform ����λ��
    m_projLoc = glGetUniformLocation(m_programID, "uProjection");
    m_viewLoc = glGetUniformLocation(m_programID, "uView");
}

ParticleShader::~ParticleShader()
{
    if (m_programID)
    {
        glDeleteProgram(m_programID);
        m_programID = 0;
    }
}

void ParticleShader::use() const
{
    glUseProgram(m_programID);
}

void ParticleShader::setProjection(const glm::mat4& projection) const
{
    // �� use() ������ Uniform
    use();
    if (m_projLoc >= 0)
    {
        glUniformMatrix4fv(m_projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    }
}

void ParticleShader::setView(const glm::mat4& view) const
{
    use();
    if (m_viewLoc >= 0)
    {
        glUniformMatrix4fv(m_viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    }
}

unsigned int ParticleShader::compileShader(const char* code, GLenum type)
{
    unsigned int shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, &code, nullptr);
    glCompileShader(shaderID);

    // ���������
    int success;
    char infoLog[512];
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
        std::string shaderType = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
        std::cerr << "[Shader Compilation Error][" << shaderType << "]: " << infoLog << std::endl;
    }
    return shaderID;
}

unsigned int ParticleShader::linkProgram(unsigned int vs, unsigned int fs)
{
    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // ������Ӵ���
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "[Program Linking Error]: " << infoLog << std::endl;
    }
    return program;
}
