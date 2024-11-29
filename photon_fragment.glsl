#version 330 core
in vec4 particleColor; // 从顶点着色器传递的颜色

out vec4 FragColor;

void main() {
    // 输出粒子的颜色
    FragColor = particleColor;
}