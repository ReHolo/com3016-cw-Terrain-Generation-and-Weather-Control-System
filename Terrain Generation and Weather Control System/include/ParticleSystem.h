#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

#include "Terrain.h"

// ���ӽṹ��
struct Particle
{
    glm::vec3 position;
    glm::vec3 velocity;
	float life; // ʣ������ (��)��Ҳ���� y<0 ֮�൱�������� Remaining life (seconds), or use y<0 as expiration condition
	bool active; // �Ƿ���ʹ�ã���Ծ�� Whether in use (active)
    
};

enum class WeatherMode {
    Sunny = 0,
    Rain,
    Snow
};

class ParticleSystem
{
public:
    ParticleSystem();
    ~ParticleSystem();

    void attachWindow(GLFWwindow* window);
    void setTerrain(Terrain* terrain);

	// �������������  Set the maximum number of particles
    void setMaxParticles(int maxCount);

	// ������������(����/��) Set the generation rate (particles/second)
    void setSpawnRate(float spawnRate);

	// ��ʼ��(���� VAO/VBO��) Initialize (create VAO/VBO, etc.)
    void init();

	// ÿ֡���� & ���� Update & render
    void update(float deltaTime);
    void render(const glm::mat4& projection, const glm::mat4& view);

private:
	// �ڲ����� Internal methods
	void processInput();           // �������  Process keyboard 
	void spawnParticle();          // ����һ�������� Spawn a new particle
	void resetParticle(Particle& p); // ����(����)ĳ������ Reset (reuse) a particle
    void updateParticles(float deltaTime);

    // ���� & ������ɫ�� Compile & link shaders
    unsigned int createShaderProgram(const char* vsSrc, const char* fsSrc);

private:
    GLFWwindow* m_window;
    Terrain* m_terrain;

    unsigned int m_programID;
    unsigned int m_VAO;
    unsigned int m_VBO;

    std::vector<Particle> m_particles;
    int  m_maxParticles;    // ���ӳش�С Particle pool size
    float m_spawnRate;      // ÿ�����ɶ������� How many particles are produced per second
	float m_spawnAccum;     // �ۻ�ʱ��, �������ɼ�� Accumulated time, used for generation interval

    WeatherMode m_currentMode;
    bool   m_shouldRender;
    glm::vec4 m_currentColor;

    float m_lastFrame;
};
