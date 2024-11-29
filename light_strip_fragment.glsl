#version 330 core
out vec4 FragColor;

uniform float factor;

void main() {
    float slowFactor = factor * 0.5;
    // 基于时间和坐标生成一个周期性的彩虹色
    vec3 rainbowColor = vec3(
        0.5 + 0.5 * sin(slowFactor + gl_FragCoord.x * 0.1),  // 红色分量
        0.5 + 0.5 * sin(slowFactor + gl_FragCoord.x * 0.1 + 2.0),  // 绿色分量
        0.5 + 0.5 * sin(slowFactor + gl_FragCoord.x * 0.1 + 4.0)   // 蓝色分量
    );
    FragColor = vec4(rainbowColor, 1.0);
}
