#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    float pointSize = 3.0;
    vec3 viewPos = vec3(view * model * vec4(aPos, 1.0)); // ��ת������ͼ�ռ�
    float distance = length(viewPos); // �������
    gl_PointSize = pointSize / (distance * 1.0); // �ɸ�����Ҫ���õ��С
}
