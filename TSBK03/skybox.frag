#version 430 core

in vec3 texCoords;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

uniform samplerCube texUnit;

void main()
{
	vec4 objColor = texture(texUnit, -texCoords);

	//fragColor = vec4(texCoords, 1.0);
	//fragColor = vec4(1.0,1.0,1.0,1.0);
	fragColor = vec4(objColor.rgb, 1.0);
	brightColor = vec4(0.f,0.f,0.f,0.f);
}