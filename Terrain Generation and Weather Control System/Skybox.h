#ifndef SKYBOX_H
#define SKYBOX_H

#include <glm/glm.hpp>
#include <vector>
#include <string>

class Skybox {
public:
    Skybox(const std::vector<std::string>& faces);
    ~Skybox();

    void render(const glm::mat4& view, const glm::mat4& projection);

private:
    unsigned int vao, vbo;
    unsigned int cubemapTexture;

    unsigned int loadCubemap(const std::vector<std::string>& faces);
};

#endif
