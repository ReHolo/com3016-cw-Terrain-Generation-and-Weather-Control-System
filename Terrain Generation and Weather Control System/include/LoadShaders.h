#ifndef LOADSHADERS_H
#define LOADSHADERS_H

#include <glad/glad.h>

struct ShaderInfo {
    GLenum type;           // ��ɫ�����ͣ�GL_VERTEX_SHADER �� GL_FRAGMENT_SHADER��
    const char* filename;  // ��ɫ���ļ�·��
    GLuint shader;         // ��ɫ������
};

GLuint LoadShaders(ShaderInfo* shaders);


#endif
