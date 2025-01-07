#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include "Terrain.h"
#include <GLFW/glfw3.h>
#include <string>


class MenuManager {
public:
    MenuManager(GLFWwindow* window, Terrain& terrain);

    void processInput();  // 处理键盘输入调整地形参数
    void render(GLFWwindow* window);    // 渲染菜单信息

private:
    GLFWwindow* window;   // 窗口引用
    Terrain& terrain;     // 地形引用

    int gridSize;         // 网格大小
    float noiseScale;     // 噪声频率
    float heightScale;    // 高度比例

    int lastGridSize;     // 上一次的网格大小
    float lastNoiseScale; // 上一次的噪声频率
    float lastHeightScale;// 上一次的高度比例

    float lastKeyPressTime; // 上一次按键响应的时间戳

    void renderMenu();    // 渲染菜单逻辑
    void updateWindowTitle(); // 更新窗口标题
};

#endif
