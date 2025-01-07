#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include "Terrain.h"
#include <GLFW/glfw3.h>
#include <string>


class MenuManager {
public:
    MenuManager(GLFWwindow* window, Terrain& terrain);

    void processInput();  // �����������������β���
    void render(GLFWwindow* window);    // ��Ⱦ�˵���Ϣ

private:
    GLFWwindow* window;   // ��������
    Terrain& terrain;     // ��������

    int gridSize;         // �����С
    float noiseScale;     // ����Ƶ��
    float heightScale;    // �߶ȱ���

    int lastGridSize;     // ��һ�ε������С
    float lastNoiseScale; // ��һ�ε�����Ƶ��
    float lastHeightScale;// ��һ�εĸ߶ȱ���

    float lastKeyPressTime; // ��һ�ΰ�����Ӧ��ʱ���

    void renderMenu();    // ��Ⱦ�˵��߼�
    void updateWindowTitle(); // ���´��ڱ���
};

#endif
