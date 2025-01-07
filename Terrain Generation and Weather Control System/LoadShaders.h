#ifndef LOADSHADERS_H
#define LOADSHADERS_H

#include <glad/glad.h>

struct ShaderInfo {
    GLenum type;           // 着色器类型（GL_VERTEX_SHADER 或 GL_FRAGMENT_SHADER）
    const char* filename;  // 着色器文件路径
    GLuint shader;         // 着色器对象
};

GLuint LoadShaders(ShaderInfo* shaders);


#endif
