#version 330 core
in vec4 particleColor; // �Ӷ�����ɫ�����ݵ���ɫ

out vec4 FragColor;

void main() {
    // ������ӵ���ɫ
    FragColor = particleColor;
}