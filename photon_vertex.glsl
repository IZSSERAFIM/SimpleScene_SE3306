#version 330 core
layout (location = 0) in vec3 aPos; // 粒子位置
layout (location = 1) in vec4 aColor; // 粒子颜色

out vec4 particleColor; // 传递给片段着色器的颜色

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    float pointSize = 0.1;
    vec3 viewPos = vec3(view * model * vec4(aPos, 1.0)); // 先转换到视图空间
    float distance = length(viewPos); // 计算距离
    gl_PointSize = pointSize / (distance * 1.0); // 可根据需要设置点大小
    particleColor = aColor;
}