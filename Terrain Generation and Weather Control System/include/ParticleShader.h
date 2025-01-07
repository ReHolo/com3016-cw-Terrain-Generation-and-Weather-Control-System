#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class ParticleShader
{
public:
    // 构造 & 析构
    ParticleShader(const std::string& vertexSrc, const std::string& fragmentSrc);
    ~ParticleShader();

    // 使用着色器
    void use() const;

    // 设置 Uniform：投影矩阵
    void setProjection(const glm::mat4& projection) const;
    // 设置 Uniform：视图矩阵
    void setView(const glm::mat4& view) const;
    // 也可以添加更多，例如颜色、点大小等

    // 获取着色器 Program
    unsigned int getProgramID() const { return m_programID; }

private:
    // 着色器程序 ID
    unsigned int m_programID;

    // 工具方法：编译、链接着色器
    unsigned int compileShader(const char* code, GLenum type);
    unsigned int linkProgram(unsigned int vs, unsigned int fs);

    // Uniform 变量在着色器中的位置（可根据实际需要添加）
    int m_projLoc;
    int m_viewLoc;
};
