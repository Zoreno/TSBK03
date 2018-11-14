#version 430 core

layout (location = 0) in vec3 in_Position;

out vec3 texCoords;

uniform mat4 mvp;

void main()
{
	vec4 world_pos = mvp*vec4(in_Position, 1.0);

	gl_Position = world_pos.xyww; 

	texCoords = in_Position;
}