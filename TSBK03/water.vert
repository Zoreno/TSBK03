#version 430 core

layout (location = 0) in vec3 in_Position;

out vec3 fragPos;

uniform mat4 models[100];

void main()
{
	mat4 model = models[gl_InstanceID];

	vec4 pos = model * vec4(in_Position, 1.0);

	fragPos = pos.xyz;
}