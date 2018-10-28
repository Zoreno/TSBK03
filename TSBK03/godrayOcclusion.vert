#version 430 core

layout (location = 0) in vec3 in_Position;

uniform mat4 mvp;

void main()
{
	vec4 pos = mvp * vec4(in_Position, 1.0);

	gl_Position = pos;
}