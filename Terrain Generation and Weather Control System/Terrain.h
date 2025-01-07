#ifndef TERRAIN_H
#define TERRAIN_H

#include <glm/glm.hpp>
#include <vector>

class Terrain {
public:
    Terrain();
    void init();
    void render();
    void updateParameters(int gridSize, float noiseScale, float heightScale);

    int getGridSize() const { return gridSize; }

private:
    unsigned int VAO, VBO, EBO;
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    int gridSize;
    float noiseScale;
    float heightScale;

    void generateTerrain();
    void setupMesh();
    float fractalNoise(float x, float z);
    float smoothHeight(float x, float z, float height);

};

#endif
