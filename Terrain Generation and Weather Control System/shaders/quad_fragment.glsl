#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(1.0, 0.0, 0.0, 1.0); // 直接返回纹理颜色
}
