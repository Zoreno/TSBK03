#version 430 core

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;

out vec3 surface_normal;

void main()
{
	gl_Position = vec4(in_Position, 1.0);
	surface_normal = in_Normal;
}