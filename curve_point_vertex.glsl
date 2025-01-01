#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aOffset;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float scale;

void main()
{
    vec3 pos = aPos * scale + aOffset;
    gl_Position = projection * view * model * vec4(pos, 1.0);
}