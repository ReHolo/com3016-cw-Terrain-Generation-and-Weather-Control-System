// Signature.h
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class Signature {
public:
    Signature();
    ~Signature();
    bool init();
    void render(const glm::mat4& projection, const glm::mat4& view, float windowWidth, float windowHeight);

private:
    unsigned int VAO, VBO;
    unsigned int textureID;
    unsigned int shaderProgram;

    unsigned int loadShaders(const char* vertexSrc, const char* fragmentSrc);
    unsigned int loadTexture(const char* path);
};
