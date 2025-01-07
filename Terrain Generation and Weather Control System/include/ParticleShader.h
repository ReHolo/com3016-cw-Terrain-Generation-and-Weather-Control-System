#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class ParticleShader
{
public:
    // ���� & ����
    ParticleShader(const std::string& vertexSrc, const std::string& fragmentSrc);
    ~ParticleShader();

    // ʹ����ɫ��
    void use() const;

    // ���� Uniform��ͶӰ����
    void setProjection(const glm::mat4& projection) const;
    // ���� Uniform����ͼ����
    void setView(const glm::mat4& view) const;
    // Ҳ������Ӹ��࣬������ɫ�����С��

    // ��ȡ��ɫ�� Program
    unsigned int getProgramID() const { return m_programID; }

private:
    // ��ɫ������ ID
    unsigned int m_programID;

    // ���߷��������롢������ɫ��
    unsigned int compileShader(const char* code, GLenum type);
    unsigned int linkProgram(unsigned int vs, unsigned int fs);

    // Uniform ��������ɫ���е�λ�ã��ɸ���ʵ����Ҫ��ӣ�
    int m_projLoc;
    int m_viewLoc;
};
