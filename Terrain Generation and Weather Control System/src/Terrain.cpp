#include "Terrain.h"
#include <glad/glad.h>
#include <glm/gtc/noise.hpp>
#include <iostream>


Terrain::Terrain()
    : VAO(0), VBO(0), EBO(0),
    gridSize(100),    
    noiseScale(0.1f),
    heightScale(20.0f)
{
}


void Terrain::init() {
    generateTerrain();
    setupMesh();
}

// 更新参数并重新生成 Terrain Update parameters and regenerate Terrain
void Terrain::updateParameters(int newGridSize, float newNoiseScale, float newHeightScale) {
    gridSize = newGridSize;
    noiseScale = newNoiseScale;
    heightScale = newHeightScale;

    generateTerrain();
    setupMesh();
}

void Terrain::generateTerrain() {
    vertices.clear();
    indices.clear();

	// 生成 (gridSize+1)*(gridSize+1) 个顶点 (x, height, z) Generate (gridSize+1)*(gridSize+1) vertices (x, height, z)
    for (int z = 0; z <= gridSize; ++z) {
        for (int x = 0; x <= gridSize; ++x) {
			// fractalNoise -> 基础噪声 (0, 1) fractalNoise -> base noise (0, 1)
            float baseHeight = fractalNoise(x * noiseScale, z * noiseScale) * heightScale;
			// 进一步平滑 (smoothHeight) Further smoothing (smoothHeight)
            float height = smoothHeight(x * noiseScale, z * noiseScale, baseHeight);

			// 存储顶点 (x, height, z) 到 vertices Store vertex (x, height, z) to vertices
            vertices.emplace_back(glm::vec3(x, height, z));
        }
    }

    // 生成索引 (两个三角形拼一个方块)  Generate indices (two triangles form one square)
    for (int z = 0; z < gridSize; ++z) {
        for (int x = 0; x < gridSize; ++x) {
            int topLeft = z * (gridSize + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (gridSize + 1) + x;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    std::cout << "Generated Smooth Terrain, gridSize=" << gridSize << std::endl;
}

// 分形噪声  Fractal noise 
float Terrain::fractalNoise(float x, float z) {
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
	float persistence = 0.4f; // 每层噪声振幅衰减系数 (persistence)
	int octaves = 3;         // 噪声叠加层数 (octaves)

    for (int i = 0; i < octaves; ++i) {
        float noiseValue = glm::simplex(glm::vec2(x * frequency, z * frequency));
        total += noiseValue * amplitude;

        frequency *= 2.0f;
        amplitude *= persistence;
    }

	// 归一化到 [0, 1] Normalize to [0, 1]
    return glm::clamp((total + 1.0f) / 2.0f, 0.0f, 1.0f);
}

// 平滑函数 Smoothing function
float Terrain::smoothHeight(float x, float z, float height) {
    float smoothFactor = 0.7f;
    float base = glm::simplex(glm::vec2(x, z)) * heightScale * 0.3f;
    return glm::mix(base, height, smoothFactor);
}

// 创建/更新 VAO VBO EBO Create/update VAO VBO EBO
void Terrain::setupMesh() {
    if (!VAO) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
    }

    glBindVertexArray(VAO);

    // VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// 顶点属性：位置 (vec3) Vertex attribute: position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

// 绘制地形 Render terrain
void Terrain::render() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
