#version 430 core

uniform vec3 color;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

void main()
{
	FragColor = vec4(color, 1.0);
	BrightColor = vec4(0.0, 0.0, 0.0, 0.0);
}