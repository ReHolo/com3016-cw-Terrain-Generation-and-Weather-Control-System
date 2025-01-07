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

// ���²������������� Terrain Update parameters and regenerate Terrain
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

	// ���� (gridSize+1)*(gridSize+1) ������ (x, height, z) Generate (gridSize+1)*(gridSize+1) vertices (x, height, z)
    for (int z = 0; z <= gridSize; ++z) {
        for (int x = 0; x <= gridSize; ++x) {
			// fractalNoise -> �������� (0, 1) fractalNoise -> base noise (0, 1)
            float baseHeight = fractalNoise(x * noiseScale, z * noiseScale) * heightScale;
			// ��һ��ƽ�� (smoothHeight) Further smoothing (smoothHeight)
            float height = smoothHeight(x * noiseScale, z * noiseScale, baseHeight);

			// �洢���� (x, height, z) �� vertices Store vertex (x, height, z) to vertices
            vertices.emplace_back(glm::vec3(x, height, z));
        }
    }

    // �������� (����������ƴһ������)  Generate indices (two triangles form one square)
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

// ��������  Fractal noise 
float Terrain::fractalNoise(float x, float z) {
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
	float persistence = 0.4f; // ÿ���������˥��ϵ�� (persistence)
	int octaves = 3;         // �������Ӳ��� (octaves)

    for (int i = 0; i < octaves; ++i) {
        float noiseValue = glm::simplex(glm::vec2(x * frequency, z * frequency));
        total += noiseValue * amplitude;

        frequency *= 2.0f;
        amplitude *= persistence;
    }

	// ��һ���� [0, 1] Normalize to [0, 1]
    return glm::clamp((total + 1.0f) / 2.0f, 0.0f, 1.0f);
}

// ƽ������ Smoothing function
float Terrain::smoothHeight(float x, float z, float height) {
    float smoothFactor = 0.7f;
    float base = glm::simplex(glm::vec2(x, z)) * heightScale * 0.3f;
    return glm::mix(base, height, smoothFactor);
}

// ����/���� VAO VBO EBO Create/update VAO VBO EBO
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

	// �������ԣ�λ�� (vec3) Vertex attribute: position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

// ���Ƶ��� Render terrain
void Terrain::render() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
