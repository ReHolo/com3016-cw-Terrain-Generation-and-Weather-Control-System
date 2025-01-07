#include "ParticleSystem.h"
#include <iostream>
#include <cstdlib>  // rand
#include <ctime>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 顶点着色器源码（带简单投影&视图） Vertex shader source code (with simple projection & view)
static const char* gVertexShaderSrc = R"(
#version 330 core
layout(location=0) in vec3 aPos;

uniform mat4 uProjection;
uniform mat4 uView;

void main()
{
    gl_Position = uProjection * uView * vec4(aPos, 1.0);
    gl_PointSize =3.0; // 点大小
}
)";

// 片段着色器源码（带颜色 uniform） Fragment shader source code (with color uniform)
static const char* gFragmentShaderSrc = R"(
#version 330 core
out vec4 FragColor;

uniform vec4 uColor;

void main()
{
    FragColor = uColor;
}
)";

ParticleSystem::ParticleSystem()
    : m_window(nullptr),
    m_terrain(nullptr),
    m_programID(0),
    m_VAO(0),
    m_VBO(0),
    m_maxParticles(15000),  
    m_spawnRate(500.f),     
    m_spawnAccum(0.f),
    m_currentMode(WeatherMode::Sunny),
    m_shouldRender(true),
    m_currentColor(1.0f),
    m_lastFrame(0.f)
{
    srand(static_cast<unsigned int>(time(nullptr)));
}

ParticleSystem::~ParticleSystem()
{
    if (m_programID) {
        glDeleteProgram(m_programID);
    }
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void ParticleSystem::attachWindow(GLFWwindow* window)
{
    m_window = window;
}

void ParticleSystem::setTerrain(Terrain* terrain)
{
    m_terrain = terrain;
}

void ParticleSystem::setMaxParticles(int maxCount)
{
    m_maxParticles = maxCount;
}

void ParticleSystem::setSpawnRate(float spawnRate)
{
    m_spawnRate = spawnRate;
}

/**
 * 初始化：创建粒子池（全部 inactive），编译着色器，创建VAO/VBO  Initialization: Create particle pool (all inactive), compile shader, create VAO/VBO   
 */
void ParticleSystem::init()
{
	// 1) 创建粒子池 (m_maxParticles )
    m_particles.clear();
    m_particles.resize(m_maxParticles);

    // 2) 初始全部 inactive
    for (auto& p : m_particles)
    {
        p.active = false;
        p.life = 0.f;
    }

	// 3) 编译链接着色器 Compile & link shaders
    if (!m_programID) {
        m_programID = createShaderProgram(gVertexShaderSrc, gFragmentShaderSrc);
    }

	// 4) 创建 VAO/VBO Create VAO/VBO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    // 分配容量: m_maxParticles * sizeof(Particle)
    glBufferData(GL_ARRAY_BUFFER, m_maxParticles * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);

	// 配置顶点属性：位置 (3个float)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

/**
 * 每帧更新：处理按键 + 生成 + 更新粒子 Update per frame: process key + spawn + update particles
 */
void ParticleSystem::update(float deltaTime)
{
    processInput();
    updateParticles(deltaTime);
}

/**
 * 绘制（只绘制active粒子，但此处为简化可先上传全部，在VS/FS里不区分） Render (only render active particles, but here for simplicity, upload all first, no distinction in VS/FS)
 */
void ParticleSystem::render(const glm::mat4& projection, const glm::mat4& view)
{
    if (!m_shouldRender) return;

	// 上传粒子数据到VBO Upload particle data to VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_maxParticles * sizeof(Particle), &m_particles[0]);

	// 使用着色器 Use shader
    glUseProgram(m_programID);

	// 设置uniform: 投影、视图、颜色 Set uniforms: projection, view, color
    int locProj = glGetUniformLocation(m_programID, "uProjection");
    if (locProj >= 0) glUniformMatrix4fv(locProj, 1, GL_FALSE, glm::value_ptr(projection));

    int locView = glGetUniformLocation(m_programID, "uView");
    if (locView >= 0) glUniformMatrix4fv(locView, 1, GL_FALSE, glm::value_ptr(view));

    int locColor = glGetUniformLocation(m_programID, "uColor");
    if (locColor >= 0) glUniform4fv(locColor, 1, glm::value_ptr(m_currentColor));

    glBindVertexArray(m_VAO);

	// 此处简单地画出全部粒子(0~m_maxParticles) Simply draw all particles (0~m_maxParticles)
    
    glDrawArrays(GL_POINTS, 0, m_maxParticles);

    glBindVertexArray(0);
}


void ParticleSystem::processInput()
{
    if (!m_window) return;

    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, true);

    if (glfwGetKey(m_window, GLFW_KEY_1) == GLFW_PRESS)
        m_currentMode = WeatherMode::Sunny;
    if (glfwGetKey(m_window, GLFW_KEY_2) == GLFW_PRESS)
        m_currentMode = WeatherMode::Rain;
    if (glfwGetKey(m_window, GLFW_KEY_3) == GLFW_PRESS)
        m_currentMode = WeatherMode::Snow;
}


void ParticleSystem::updateParticles(float deltaTime)
{
	// 根据模式设颜色 According to mode, set color
    switch (m_currentMode)
    {
    case WeatherMode::Rain:
        m_currentColor = glm::vec4(0.f, 0.f, 1.f, 1.f); // 蓝色
        break;
    case WeatherMode::Snow:
        m_currentColor = glm::vec4(1.f, 1.f, 1.f, 1.f); // 白色
        break;
    default: // Sunny
        m_currentColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
        break;
    }

	// 晴天 => 不渲染 & 不 spawn Sunny => no render & no spawn
    m_shouldRender = (m_currentMode != WeatherMode::Sunny);

	// 1) 生成 (Rain/Snow)  Spawn (Rain/Snow)
    if (m_currentMode != WeatherMode::Sunny)
    {
        float numToSpawn = m_spawnRate * deltaTime; // 每帧需要生成多少
        m_spawnAccum += numToSpawn;

        while (m_spawnAccum >= 1.f)
        {
            spawnParticle();
            m_spawnAccum -= 1.f;
        }
    }

	// 2) 更新已有粒子 Update existing particles
    int gs = (m_terrain ? m_terrain->getGridSize() : 100);

    for (auto& p : m_particles)
    {
        if (!p.active) continue;

		// 下雨 => 快速, 带随机速度 Rain => fast, with random speed
        if (m_currentMode == WeatherMode::Rain)
        {
			// 例如: 基础向下速度(-10)，加入 ±2 的随机 offset
            float baseSpeed = -10.f;
            float randomOffset = ((rand() % 100) / 100.f) * 4.f - 2.f; // -2 ~ +2
            p.velocity.y = baseSpeed + randomOffset;

			// 水平也可有小随机 offset
            float xRand = ((rand() % 100) - 50) / 100.f; // -0.5 ~ +0.5
            float zRand = ((rand() % 100) - 50) / 100.f;
            p.velocity.x = xRand;
            p.velocity.z = zRand;
        }
        else if (m_currentMode == WeatherMode::Snow)
        {
			// 雪花速度更慢, y=-2 ± 1 Snow is slower, y=-2 ± 1
            float baseSpeed = -2.f;
            float randomOffset = ((rand() % 100) / 100.f) * 2.f - 1.f; // -1 ~ +1
            p.velocity.y = baseSpeed + randomOffset;

			// x,z 也可能有大一些偏移 (±0.25)
            float xRand = ((rand() % 100) - 50) / 200.f; // -0.25 ~ +0.25
            float zRand = ((rand() % 100) - 50) / 200.f;
            p.velocity.x = xRand;
            p.velocity.z = zRand;
        }
        else
        {
			// 晴天 or else => 不动 Sunny or else => no movement
            p.velocity = glm::vec3(0.f);
        }

		// 移动 Move
        p.position += p.velocity * deltaTime;

		// 寿命衰减 Life decay
        p.life -= deltaTime;
        if (p.life <= 0.f)
        {
            p.active = false;
            continue;
        }

		// 若超出地形范围 or y<0 => inactive If out of terrain range or y<0 => inactive
        if (p.position.y < 0.f || p.position.x<0.f || p.position.x>gs
            || p.position.z<0.f || p.position.z>gs)
        {
            p.active = false;
        }
    }
}

/**
 * 生成(激活)一个粒子:
 *  - 找到一个 inactive 的位置 -> reset
 */
void ParticleSystem::spawnParticle()
{
    for (auto& p : m_particles)
    {
        if (!p.active)
        {
            resetParticle(p);
            return; 
        }
    }
	// 如果池满 => 忽略 (不生成) If pool is full => ignore (no spawn)
}

/**
 * 重置粒子：随机位置(上方)、随机寿命等 Reset particle: random position (above), random life, etc.
 */
void ParticleSystem::resetParticle(Particle& p)
{
    p.active = true;
    p.life = 10.0f;  

    int gs = (m_terrain ? m_terrain->getGridSize() : 100);
    float rx = static_cast<float>(rand() % (gs + 1));
    float rz = static_cast<float>(rand() % (gs + 1));
	// 固定 y=30 让它从上往下掉 Fixed y=30 to let it fall from above
    p.position = glm::vec3(rx, 30.f, rz);
    p.velocity = glm::vec3(0.f);
}

/** 编译 & 链接着色器，与以前类似  Compile & link shaders, similar to before */  
unsigned int ParticleSystem::createShaderProgram(const char* vsSrc, const char* fsSrc)
{
    auto compileShader = [&](const char* code, GLenum type)->GLuint {
        GLuint s = glCreateShader(type);
        glShaderSource(s, 1, &code, nullptr);
        glCompileShader(s);
        int success;
        glGetShaderiv(s, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(s, 512, nullptr, infoLog);
            std::cerr << "Shader compile error("
                << (type == GL_VERTEX_SHADER ? "VERT" : "FRAG")
                << "): " << infoLog << std::endl;
            return 0;
        }
        return s;
        };

    GLuint vs = compileShader(vsSrc, GL_VERTEX_SHADER);
    GLuint fs = compileShader(fsSrc, GL_FRAGMENT_SHADER);
    if (!vs || !fs) return 0;

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    int linked;
    glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (!linked) {
        char infoLog[512];
        glGetProgramInfoLog(prog, 512, nullptr, infoLog);
        std::cerr << "Program link error: " << infoLog << std::endl;
        glDeleteShader(vs);
        glDeleteShader(fs);
        return 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}
