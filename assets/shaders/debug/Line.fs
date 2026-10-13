#version 330 core
out vec4 FragColor;

uniform bool drawBounds;
uniform vec4 lineColor;

void main()
{
    FragColor = lineColor;
}