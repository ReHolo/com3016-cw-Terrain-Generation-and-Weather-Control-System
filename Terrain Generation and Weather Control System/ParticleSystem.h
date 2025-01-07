#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

#include "Terrain.h"

// 粒子结构体
struct Particle
{
    glm::vec3 position;
    glm::vec3 velocity;
	float life; // 剩余寿命 (秒)，也可用 y<0 之类当过期条件 Remaining life (seconds), or use y<0 as expiration condition
	bool active; // 是否在使用（活跃） Whether in use (active)
    
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

	// 设置最大粒子数  Set the maximum number of particles
    void setMaxParticles(int maxCount);

	// 设置生成速率(粒子/秒) Set the generation rate (particles/second)
    void setSpawnRate(float spawnRate);

	// 初始化(创建 VAO/VBO等) Initialize (create VAO/VBO, etc.)
    void init();

	// 每帧更新 & 绘制 Update & render
    void update(float deltaTime);
    void render(const glm::mat4& projection, const glm::mat4& view);

private:
	// 内部方法 Internal methods
	void processInput();           // 处理键盘  Process keyboard 
	void spawnParticle();          // 生成一个新粒子 Spawn a new particle
	void resetParticle(Particle& p); // 重置(复用)某个粒子 Reset (reuse) a particle
    void updateParticles(float deltaTime);

    // 编译 & 链接着色器 Compile & link shaders
    unsigned int createShaderProgram(const char* vsSrc, const char* fsSrc);

private:
    GLFWwindow* m_window;
    Terrain* m_terrain;

    unsigned int m_programID;
    unsigned int m_VAO;
    unsigned int m_VBO;

    std::vector<Particle> m_particles;
    int  m_maxParticles;    // 粒子池大小 Particle pool size
    float m_spawnRate;      // 每秒生成多少粒子 How many particles are produced per second
	float m_spawnAccum;     // 累积时间, 用于生成间隔 Accumulated time, used for generation interval

    WeatherMode m_currentMode;
    bool   m_shouldRender;
    glm::vec4 m_currentColor;

    float m_lastFrame;
};
