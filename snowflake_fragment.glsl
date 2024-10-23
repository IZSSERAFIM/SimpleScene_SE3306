#version 330 core

out vec4 FragColor;

uniform vec4 objectColor;

void main() {
    // ���㵱ǰƬ������ڵ����ĵ�����
    vec2 coord = gl_PointCoord - vec2(0.5);
    // �ж��Ƿ���Բ�İ뾶�ڣ��������������Ƭ��
    if (dot(coord, coord) > 0.25) {
        discard;
    }
    
    FragColor = vec4(objectColor); 
}
