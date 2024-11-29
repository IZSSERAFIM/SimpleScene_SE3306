#version 330 core
layout (location = 0) in vec3 aPos; // ����λ��
layout (location = 1) in vec4 aColor; // ������ɫ

out vec4 particleColor; // ���ݸ�Ƭ����ɫ������ɫ

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    float pointSize = 0.1;
    vec3 viewPos = vec3(view * model * vec4(aPos, 1.0)); // ��ת������ͼ�ռ�
    float distance = length(viewPos); // �������
    gl_PointSize = pointSize / (distance * 1.0); // �ɸ�����Ҫ���õ��С
    particleColor = aColor;
}