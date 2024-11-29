/*
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out float Height;
out vec3 Position;
out vec2 TexCoords;
out vec4 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float gMinHeight;
uniform float gMaxHeight;

void main()
{
    Height = aPos.y;
    Position = (view * model * vec4(aPos, 1.0)).xyz;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoords = aTexCoords;
    float DeltaHeight = gMaxHeight - gMinHeight;
    float HeightRatio = (Position.y - gMinHeight) / DeltaHeight;
    float c = HeightRatio * 0.4 + 0.6;
    Color = vec4(c, c, c, 1.0);
}
*/
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out float Height;
out vec3 Position;
out vec2 TexCoords;
out vec4 Color;
out float isInsideCircle; // 新增变量，表示是否在圆形区域内

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float gMinHeight;
uniform float gMaxHeight;

void main()
{
    // 计算当前顶点到原点的距离
    float dist = length(aPos.xz);  // 只考虑x和z坐标，因为是圆形地形

    Height = aPos.y;
    Position = (view * model * vec4(aPos, 1.0)).xyz;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoords = aTexCoords;

    float DeltaHeight = gMaxHeight - gMinHeight;
    float HeightRatio = (Position.y - gMinHeight) / DeltaHeight;
    float c = HeightRatio * 0.4 + 0.6;
    Color = vec4(c, c, c, 1.0);
}