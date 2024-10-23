#version 330 core

out vec4 FragColor;

uniform vec4 objectColor;

void main() {
    // 计算当前片段相对于点中心的坐标
    vec2 coord = gl_PointCoord - vec2(0.5);
    // 判断是否在圆的半径内，如果超出则丢弃该片段
    if (dot(coord, coord) > 0.25) {
        discard;
    }
    
    FragColor = vec4(objectColor); 
}
