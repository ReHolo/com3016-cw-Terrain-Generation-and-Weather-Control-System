#version 330 core
out vec4 FragColor;

uniform vec3 particleColor; // 粒子颜色

void main() {
    // 设置为粒子的颜色（白色）
    FragColor = vec4(particleColor, 1.0); // 雪花颜色为不透明的白色
}
