#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "Terrain.h"
#include "LoadShaders.h"
#include "TextureLoader.h"
#include <iostream>
#include "Skybox.h"
#include "MenuManager.h"
#include "ParticleSystem.h"






// ȫ�ֱ���
Camera camera(glm::vec3(0.0f, 20.0f, 50.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;
float lastX = 400.0f, lastY = 300.0f;




// �ص���������
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int main() {
    // ��ʼ�� GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1920,1080,"Terrain Generation with Skybox", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 1920, 1080);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);

    GLuint terrainShaderProgram, particleShaderProgram;

    // ������ɫ��
    ShaderInfo shaders[] = {
        { GL_VERTEX_SHADER, "shaders/vertex.glsl" },
        { GL_FRAGMENT_SHADER, "shaders/fragment.glsl" },
        { GL_NONE, nullptr }
    };
    GLuint shaderProgram = LoadShaders(shaders);
    if (shaderProgram == 0) {
        std::cerr << "Failed to load shaders." << std::endl;
        return -1;
    }

    

    // ����������ɫ���ݵ���ɫ��
    GLuint ambientColorLoc = glGetUniformLocation(shaderProgram, "ambientColor");
    GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    GLuint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");

    glm::vec3 ambientColor = glm::vec3(1.0f, 1.0f, 1.0f); // ������Ϊ�ϰ��Ļ�ɫ
    // �����Դ��λ�ú���ɫ
    glm::vec3 lightPos = glm::vec3(10.0f, 20.0f, 10.0f); // ��Դλ���������� (10, 20, 10)
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);  // ��ԴΪ��ɫ


    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");

    GLuint grassTexture = LoadTexture("textures/grass.jpg");
    GLuint rockTexture = LoadTexture("textures/rock.jpg");
    GLuint snowTexture = LoadTexture("textures/snow.jpg");
    if (!grassTexture || !rockTexture || !snowTexture) {
        std::cerr << "Failed to load one or more textures." << std::endl;
        return -1;
    }

    
    

    std::vector<std::string> faces = {
        "textures/skybox/right.jpg",
        "textures/skybox/left.jpg",
        "textures/skybox/top.jpg",
        "textures/skybox/bottom.jpg",
        "textures/skybox/front.jpg",
        "textures/skybox/back.jpg"
    };
    Skybox skybox(faces);
    
    Terrain terrain;
    

    MenuManager menuManager(window, terrain);

    ParticleSystem particleSystem;
    particleSystem.attachWindow(window);
    particleSystem.setMaxParticles(15000);
    // ������������(����/��)
    particleSystem.setSpawnRate(1000.f);

    // ��ʼ������ϵͳ
    particleSystem.init();

    
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),1280.0f / 720.0f, 0.1f, 1000.0f);


        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
        glUniform3fv(ambientColorLoc, 1, glm::value_ptr(ambientColor));

        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(camera.Position));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));
        glUniform3fv(glGetUniformLocation(shaderProgram, "ambientColor"), 1, glm::value_ptr(ambientColor));

        glUniform1f(glGetUniformLocation(shaderProgram, "minHeightGrass"), 0.0f);
        glUniform1f(glGetUniformLocation(shaderProgram, "maxHeightGrass"), 5.0f);
        glUniform1f(glGetUniformLocation(shaderProgram, "minHeightRock"), 4.0f);
        glUniform1f(glGetUniformLocation(shaderProgram, "maxHeightRock"), 10.0f);
        glUniform1f(glGetUniformLocation(shaderProgram, "minHeightSnow"), 9.0f);

        // ��Ⱦ��պ�
        glDepthFunc(GL_LEQUAL);  // �ı���Ⱥ�����ȷ�����ֵΪ 1.0 ������ͨ������
        glDepthMask(GL_FALSE);  // �������д��
        skybox.render(glm::mat4(glm::mat3(view)), projection);
        glDepthMask(GL_TRUE);   // �������д��
        glDepthFunc(GL_LESS);   // �ָ�Ĭ����Ⱥ���

        glDepthMask(GL_FALSE); // ��ֹ���д��
        
        glDepthMask(GL_TRUE);  // �ָ����д��

        
        // ��Ⱦ����
        glUseProgram(shaderProgram);
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "grassTexture"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, rockTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "rockTexture"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, snowTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "snowTexture"), 2);

        menuManager.processInput();
        
       

        // ��������ϵͳ
        particleSystem.update(deltaTime);
        // ��Ⱦ����
        terrain.render();

        // ��Ⱦ�˵�
        menuManager.render(window);
       
        particleSystem.render(projection, view);
        
        glfwSwapBuffers(window);
        glfwPollEvents();

        
       
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

 
		
	
}




void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}



