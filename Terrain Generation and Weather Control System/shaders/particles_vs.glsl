#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 worldPosition = projection * view * vec4(position, 1.0);

    // 设置固定深度值（0.9 保证在地形后面，天空盒前面）
    worldPosition.z = 0.9 * worldPosition.w; // 深度值映射到投影空间
    gl_Position = worldPosition;
}
