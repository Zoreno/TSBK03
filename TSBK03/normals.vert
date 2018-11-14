#version 430 core

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;

out vec3 normal;

uniform mat4 model;
uniform mat4 vp;

void main()
{
	normal = normalize((vp * transpose(inverse(model)) * vec4(in_Normal, 0.0)).xyz);

	gl_Position = vp * model * vec4(in_Position, 1.0);
}