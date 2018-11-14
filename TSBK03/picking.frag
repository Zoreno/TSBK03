#version 430 core

uniform int drawIndex;
uniform int objectIndex;

out vec3 FragColor;

void main()
{
	FragColor = vec3(float(objectIndex), float(drawIndex), float(gl_PrimitiveID + 1));
}