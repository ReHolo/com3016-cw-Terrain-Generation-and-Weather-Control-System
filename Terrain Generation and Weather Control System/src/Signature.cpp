// Signature.cpp
#include "Signature.h"
#include "signatureVertices.h" // 确保路径正确
#include <stb_image.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// 顶点着色器源码
const char* signatureVertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec3 aPos; // 顶点位置
layout(location = 1) in vec2 aTexCoord; // 纹理坐标

out vec2 TexCoord; // 传递到片段着色器

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

// 片段着色器源码
const char* signatureFragmentShaderSrc = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

void main()
{
    // 将纹理坐标从 [0,1] 范围转换为 [-1,1] 范围
    vec2 coord = TexCoord * 2.0 - 1.0;

    // 定义十字线的宽度
    float lineWidth = 0.02;  // 增加线宽使十字更明显

    // 判断片段是否在十字线内
    if (abs(coord.x) < lineWidth || abs(coord.y) < lineWidth)
    {
        // 白色十字
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
    else
    {
        // 绿色背景 (RGB: 0.0, 0.8, 0.0)
        FragColor = vec4(0.0, 0.8, 0.0, 0.8);  // 最后的0.8是透明度
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
    // 编译和链接着色器
    shaderProgram = loadShaders(signatureVertexShaderSrc, signatureFragmentShaderSrc);
    if (shaderProgram == 0) {
        std::cerr << "Failed to compile/link signature shaders." << std::endl;
        return false;
    }

    // 设置 VAO 和 VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(signatureVertices), signatureVertices, GL_STATIC_DRAW);

    // 位置属性
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // 纹理坐标属性
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);

    

    return true;
}

unsigned int Signature::loadShaders(const char* vertexSrc, const char* fragmentSrc)
{
    // 编译顶点着色器
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, NULL);
    glCompileShader(vertexShader);
    // 检查编译错误
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SIGNATURE::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // 编译片段着色器
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSrc, NULL);
    glCompileShader(fragmentShader);
    // 检查编译错误
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SIGNATURE::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // 链接着色器程序
    unsigned int shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vertexShader);
    glAttachShader(shaderProg, fragmentShader);
    glLinkProgram(shaderProg);
    // 检查链接错误
    glGetProgramiv(shaderProg, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProg, 512, NULL, infoLog);
        std::cerr << "ERROR::SIGNATURE::SHADER::LINKING_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // 删除着色器对象
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProg;
}

unsigned int Signature::loadTexture(const char* path)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 加载图像
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // 根据需要翻转
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
            format = GL_RGB; // 默认

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

    // 计算签名在屏幕上的位置和大小
    float signatureWidth = windowWidth * 0.5f;  // 签名宽度为窗口宽度的20%
    float signatureHeight = windowHeight * 0.5f; // 签名高度为窗口高度的10%
    float xPos = windowWidth - signatureWidth - 10.0f; 
    float yPos = 10.0f; 

    // 创建模型矩阵
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(xPos, yPos, 0.0f));
    model = glm::scale(model, glm::vec3(signatureWidth, signatureHeight, 1.0f));

    // 使用着色器
    glUseProgram(shaderProgram);

    // 设置 uniform
    unsigned int projLoc = glGetUniformLocation(shaderProgram, "uProjection");
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "uView");
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "uModel");

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // 启用混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(shaderProgram, "uTexture"), 0);

    // 绘制矩形
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // 禁用混合
    glDisable(GL_BLEND);

    glUseProgram(0);
}
