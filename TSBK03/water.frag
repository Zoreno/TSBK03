#version 430 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

#define MAX_LIGHTS 8

flat in vec3 normal_es;

uniform vec3 lightDir[MAX_LIGHTS];
uniform int numDirectionalLights = 0;

void main()
{
	vec3 norm = normalize(normal_es);
	
	vec3 lightColor = vec3(0.0, 0.0, 0.0);

	for(int i = 0; i < numDirectionalLights; ++i)
	{
		float diffFactor = max(dot(norm, -lightDir[i]), 0.0);

		lightColor += vec3(diffFactor);
	}
	
	vec3 waterColor = vec3(0.14, 0.53, 0.85);

	FragColor = vec4(waterColor * lightColor, 1.0);
	BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}