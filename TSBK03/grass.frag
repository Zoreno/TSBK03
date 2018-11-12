#version 430 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 normal_GS;
in vec3 occlusionFactor;

#define MAX_LIGHTS 8

uniform vec3 lightDir[MAX_LIGHTS];
uniform int numDirectionalLights = 0;

void main()
{
	vec3 norm = normalize(normal_GS);
	
	vec3 lightColor = vec3(0.0, 0.0, 0.0);

	for(int i = 0; i < numDirectionalLights; ++i)
	{
		vec3 lightDirNorm = normalize(lightDir[i]);

		float diffFactor = max(dot(norm, -lightDirNorm), 0.0);

		lightColor += vec3(diffFactor);
	}

	//vec3 grassColor = vec3(0.23, 0.68, 0.36);
	vec3 grassColor = vec3(0.0, 1.0, 0.0);

	FragColor = vec4(grassColor * (0.7 * lightColor + 0.3) * (occlusionFactor), 1.0);

	// TODO: Make the grass output color to the bloom buffer
	BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}