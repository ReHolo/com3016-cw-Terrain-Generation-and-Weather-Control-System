#include "MenuManager.h"
#include <iostream>

MenuManager::MenuManager(GLFWwindow* window, Terrain& terrain)
    : window(window), terrain(terrain), gridSize(50), noiseScale(0.1f), heightScale(20.0f),
    lastGridSize(50), lastNoiseScale(0.1f), lastHeightScale(20.0f), lastKeyPressTime(0.0f) {}

void MenuManager::processInput() {
    float currentTime = glfwGetTime();
    if (currentTime - lastKeyPressTime < 0.05f)return;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && gridSize < 800) {
        gridSize += 1;
        lastKeyPressTime = currentTime;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && gridSize > 10) {
        gridSize -= 1;
        lastKeyPressTime = currentTime;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && noiseScale < 5.0f) {
        noiseScale += 0.001f;
        lastKeyPressTime = currentTime;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && noiseScale > 0.0f) {
        noiseScale -= 0.001f;
        lastKeyPressTime = currentTime;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS && heightScale < 50.0f) {
        heightScale += 0.5f;
        lastKeyPressTime = currentTime;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS && heightScale > 1.0f) {
        heightScale -= 0.5f;
        lastKeyPressTime = currentTime;
    }

    if (gridSize != lastGridSize || noiseScale != lastNoiseScale || heightScale != lastHeightScale) {
        terrain.updateParameters(gridSize, noiseScale, heightScale);
        lastGridSize = gridSize;
        lastNoiseScale = noiseScale;
        lastHeightScale = heightScale;
        updateWindowTitle();
    }


}

void MenuManager::render(GLFWwindow* window) {
    renderMenu();
}

void MenuManager::renderMenu() {
    std::cout << "Rendering Menu:" << std::endl;
    std::cout << "Grid Size: " << gridSize << std::endl;
    std::cout << "Noise Scale: " << noiseScale << std::endl;
    std::cout << "Height Scale: " << heightScale << std::endl;
}

void MenuManager::updateWindowTitle() {
    std::string title = "Terrain Editor | GridSize: " + std::to_string(gridSize) +
        ", NoiseScale: " + std::to_string(noiseScale) +
        ", HeightScale: " + std::to_string(heightScale);
    glfwSetWindowTitle(window, title.c_str());
}
