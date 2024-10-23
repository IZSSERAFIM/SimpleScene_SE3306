#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float pointSize;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    // 计算摄像机到点的距离
    vec3 viewPos = vec3(view * model * vec4(aPos, 1.0)); // 先转换到视图空间
    float distance = length(viewPos); // 计算距离
    gl_PointSize = pointSize / (distance * 1.0); // 可根据需要设置点大小
}
