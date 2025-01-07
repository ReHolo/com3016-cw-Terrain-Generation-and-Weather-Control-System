#version 330 core

out vec4 FragColor;

uniform vec3 particleColor; // 动态传递粒子颜色

void main() {
    FragColor = vec4(particleColor, 1.0); // 使用传递的颜色
}
