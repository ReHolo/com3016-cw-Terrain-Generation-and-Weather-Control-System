#include "LoadShaders.h"
#include <fstream>
#include <sstream>
#include <iostream>

GLuint LoadShaders(ShaderInfo* shaders) {
    GLuint program = glCreateProgram();
    ShaderInfo* entry = shaders;

    while (entry->type != GL_NONE) {
        std::ifstream shaderFile(entry->filename);
        if (!shaderFile.is_open()) {
            std::cerr << "Failed to open shader file: " << entry->filename << std::endl;
            return 0;
        }

        std::stringstream buffer;
        buffer << shaderFile.rdbuf();
        std::string sourceCode = buffer.str();
        const char* source = sourceCode.c_str();

        GLuint shader = glCreateShader(entry->type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            char infoLog[512];
            glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "Shader compilation failed: " << infoLog << std::endl;
            return 0;
        }

        glAttachShader(program, shader);
        entry->shader = shader;
        ++entry;
    }

    glLinkProgram(program);

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        char infoLog[512];
        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Program linking failed: " << infoLog << std::endl;
        return 0;
    }

    entry = shaders;
    while (entry->type != GL_NONE) {
        glDeleteShader(entry->shader);
        ++entry;
    }

    return program;
}
