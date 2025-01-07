#version 330 core

in vec3 FragPos;          // 世界空间中的片段位置
in vec3 Normal;           // 法线
in vec2 TexCoords;        // 纹理坐标

uniform sampler2D grassTexture;
uniform sampler2D rockTexture;
uniform sampler2D snowTexture;
uniform sampler3D noiseTexture;  // 3D 噪声纹理

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform vec3 fogColor;        // 雾的颜色
uniform float fogDensity;     // 雾的密度
uniform float time;           // 时间，用于动态效果

out vec4 FragColor;

void main() {
    // --- 光照计算 ---
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 ambient = 0.8 * lightColor;
    vec3 diffuse = diff * lightColor;
    vec3 lighting = ambient + diffuse;

    // --- 材质混合 ---
    float height = FragPos.y;
    vec3 grassColor = texture(grassTexture, TexCoords).rgb;
    vec3 rockColor = texture(rockTexture, TexCoords).rgb;
    vec3 snowColor = texture(snowTexture, TexCoords).rgb;

    vec3 materialColor;
    if (height < 5.0) {
        materialColor = grassColor;
    } else if (height < 10.0) {
        float t = (height - 5.0) / 5.0;
        materialColor = mix(grassColor, rockColor, t);
    } else {
        float t = (height - 10.0) / 5.0;
        materialColor = mix(rockColor, snowColor, t);
    }

    vec3 baseColor = lighting * materialColor;

    // --- 雾效果 ---
    float fogHeightStart = 10.0; // 雾开始的高度
    float fogHeightEnd = 20.0;   // 雾结束的高度
    float fogFactor = 0.0;

    // 如果片段高度在雾的范围内，计算雾的浓度
    if (height >= fogHeightStart && height <= fogHeightEnd) {
        float fogHeightFactor = (height - fogHeightStart) / (fogHeightEnd - fogHeightStart);

        // 3D 噪声采样
        vec3 noisePos = FragPos * 0.1 + vec3(0.0, time * 0.1, 0.0); // 动态噪声偏移
        float noiseValue = texture(noiseTexture, noisePos).r;

        // 动态雾浓度计算
        fogFactor = fogHeightFactor * exp(-fogDensity * length(FragPos - viewPos) + noiseValue);
    }

    // 混合雾颜色
    vec3 foggedColor = mix(baseColor, fogColor, fogFactor);

    FragColor = vec4(foggedColor, 1.0);
}
