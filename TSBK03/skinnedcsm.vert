#version 430 core

#define MAX_BONES 100

layout (location = 0) in vec3 in_Position;

layout (location = 5) in ivec4 in_IDs;
layout (location = 6) in vec4 in_Weights;

uniform mat4 mvp;

uniform mat4 bones[MAX_BONES];

mat4 getBoneTransform()
{
	mat4 boneTransform = bones[in_IDs[0]] * in_Weights[0];

	boneTransform += bones[in_IDs[1]] * in_Weights[1];
	boneTransform += bones[in_IDs[2]] * in_Weights[2];
	boneTransform += bones[in_IDs[3]] * in_Weights[3];

	return boneTransform;
}

void main()
{
	mat4 boneTransform = getBoneTransform();

	vec4 pos = mvp * boneTransform * vec4(in_Position, 1.0);

	gl_Position = pos;
}