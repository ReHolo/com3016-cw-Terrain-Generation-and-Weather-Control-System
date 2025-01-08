// Signature.cpp
#include "Signature.h"
#include "signatureVertices.h" // ȷ��·����ȷ
#include <stb_image.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// ������ɫ��Դ��
const char* signatureVertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec3 aPos; // ����λ��
layout(location = 1) in vec2 aTexCoord; // ��������

out vec2 TexCoord; // ���ݵ�Ƭ����ɫ��

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

// Ƭ����ɫ��Դ��
const char* signatureFragmentShaderSrc = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

void main()
{
    // ����������� [0,1] ��Χת��Ϊ [-1,1] ��Χ
    vec2 coord = TexCoord * 2.0 - 1.0;

    // ����ʮ���ߵĿ��
    float lineWidth = 0.02;  // �����߿�ʹʮ�ָ�����

    // �ж�Ƭ���Ƿ���ʮ������
    if (abs(coord.x) < lineWidth || abs(coord.y) < lineWidth)
    {
        // ��ɫʮ��
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
    else
    {
        // ��ɫ���� (RGB: 0.0, 0.8, 0.0)
        FragColor = vec4(0.0, 0.8, 0.0, 0.8);  // ����0.8��͸����
    }
}
)";


Signature::Signature() : VAO(0), VBO(0), textureID(0), shaderProgram(0) {}

Signature::~Signature()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &textureID);
    glDeleteProgram(shaderProgram);
}

bool Signature::init()
{
    // �����������ɫ��
    shaderProgram = loadShaders(signatureVertexShaderSrc, signatureFragmentShaderSrc);
    if (shaderProgram == 0) {
        std::cerr << "Failed to compile/link signature shaders." << std::endl;
        return false;
    }

    // ���� VAO �� VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(signatureVertices), signatureVertices, GL_STATIC_DRAW);

    // λ������
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // ������������
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);

    

    return true;
}

unsigned int Signature::loadShaders(const char* vertexSrc, const char* fragmentSrc)
{
    // ���붥����ɫ��
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, NULL);
    glCompileShader(vertexShader);
    // ���������
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SIGNATURE::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // ����Ƭ����ɫ��
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSrc, NULL);
    glCompileShader(fragmentShader);
    // ���������
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SIGNATURE::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // ������ɫ������
    unsigned int shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vertexShader);
    glAttachShader(shaderProg, fragmentShader);
    glLinkProgram(shaderProg);
    // ������Ӵ���
    glGetProgramiv(shaderProg, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProg, 512, NULL, infoLog);
        std::cerr << "ERROR::SIGNATURE::SHADER::LINKING_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // ɾ����ɫ������
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProg;
}

unsigned int Signature::loadTexture(const char* path)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // �����������
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // ����ͼ��
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // ������Ҫ��ת
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        else
            format = GL_RGB; // Ĭ��

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Failed to load texture at path: " << path << std::endl;
        stbi_image_free(data);
        return 0;
    }

    return texture;
}

void Signature::render(const glm::mat4& projection, const glm::mat4& view, float windowWidth, float windowHeight)
{
    if (!shaderProgram) {
        if (!init()) {
            return;
        }
    }

    // ����ǩ������Ļ�ϵ�λ�úʹ�С
    float signatureWidth = windowWidth * 0.5f;  // ǩ�����Ϊ���ڿ�ȵ�20%
    float signatureHeight = windowHeight * 0.5f; // ǩ���߶�Ϊ���ڸ߶ȵ�10%
    float xPos = windowWidth - signatureWidth - 10.0f; 
    float yPos = 10.0f; 

    // ����ģ�;���
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(xPos, yPos, 0.0f));
    model = glm::scale(model, glm::vec3(signatureWidth, signatureHeight, 1.0f));

    // ʹ����ɫ��
    glUseProgram(shaderProgram);

    // ���� uniform
    unsigned int projLoc = glGetUniformLocation(shaderProgram, "uProjection");
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "uView");
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "uModel");

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // ���û��
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ������
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(shaderProgram, "uTexture"), 0);

    // ���ƾ���
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // ���û��
    glDisable(GL_BLEND);

    glUseProgram(0);
}
