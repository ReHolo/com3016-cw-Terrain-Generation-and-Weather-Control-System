#version 330 core

in vec3 FragPos;
in vec3 Normal;

uniform sampler2D grassTexture;
uniform sampler2D rockTexture;
uniform sampler2D snowTexture;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

out vec4 FragColor;

// 噪声生成函数 (简单 hash 噪声)
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    // 平滑插值
    f = f * f * (3.0 - 2.0 * f);

    float a = hash(i + vec2(0.0, 0.0));
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

void main() {
    // 法线归一化
    vec3 norm = normalize(Normal);

    // 光照方向
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    // 环境光
    vec3 ambient = 0.8 * lightColor;

    // 漫反射光
    vec3 diffuse = diff * lightColor;

    // 结合光照
    vec3 result = ambient + diffuse;

    // 高度混合纹理
    float height = FragPos.y;
    vec2 uv = FragPos.xz * 0.1;

    vec3 grassColor = texture(grassTexture, uv).rgb;
    vec3 rockColor = texture(rockTexture, uv).rgb;
    vec3 snowColor = texture(snowTexture, uv).rgb;

    vec3 materialColor;

    if (height < 5.0) {
        // 低高度区域完全是草地
        materialColor = grassColor;
    } else if (height < 15.0) {
        // 石头区域范围扩大，过渡更加陡峭
        float t = (height - 5.0) / 10.0;
        t = pow(t, 3.0); // 使用指数函数使过渡更加陡峭
        float noiseValue = noise(uv * 10.0) * 0.2; // 添加噪声影响
        materialColor = mix(grassColor, rockColor, t + noiseValue);
    } else {
        // 雪地区域，过渡陡峭
        float t = (height - 15.0) / 5.0;
        t = pow(t, 3.0); // 使用指数函数使过渡更加陡峭
        float noiseValue = noise(uv * 10.0) * 0.1; // 添加噪声影响
        materialColor = mix(rockColor, snowColor, t + noiseValue);
    }

    // 最终颜色
    FragColor = vec4(result * materialColor, 1.0);
	
	FragColor = vec4(particleColor, 1.0); // 使用传入的粒子颜色渲染
}
