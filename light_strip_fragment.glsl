#version 330 core
out vec4 FragColor;

uniform float factor;

void main() {
    float slowFactor = factor * 0.5;
    // ����ʱ�����������һ�������ԵĲʺ�ɫ
    vec3 rainbowColor = vec3(
        0.5 + 0.5 * sin(slowFactor + gl_FragCoord.x * 0.1),  // ��ɫ����
        0.5 + 0.5 * sin(slowFactor + gl_FragCoord.x * 0.1 + 2.0),  // ��ɫ����
        0.5 + 0.5 * sin(slowFactor + gl_FragCoord.x * 0.1 + 4.0)   // ��ɫ����
    );
    FragColor = vec4(rainbowColor, 1.0);
}
