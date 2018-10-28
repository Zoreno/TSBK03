//=============================================================================
// Vertex Shader
//=============================================================================

#version 430 core

//=============================================================================
// Types
//=============================================================================

#define NUM_CASCADES 3
#define MAX_LIGHTS 8

#define MAX_BONES 100

//=============================================================================
// Input
//=============================================================================

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_TexCoord;
layout (location = 3) in vec3 in_Tangents;
layout (location = 4) in vec3 in_Bitangents;
layout (location = 5) in ivec4 in_IDs;
layout (location = 6) in vec4 in_Weights;

//=============================================================================
// Uniforms
//=============================================================================

uniform mat4 model;
uniform mat4 mvp;
uniform mat4 bones[MAX_BONES];

uniform mat4 lightMVP[NUM_CASCADES * MAX_LIGHTS];

//=============================================================================
// Output 
//=============================================================================

out vec3 normal;
out vec3 tangent;
out vec3 bitangent;
out vec3 fragPos;
out vec2 texCoords;

out vec4 lightSpacePos[NUM_CASCADES * MAX_LIGHTS];
out float clipSpaceZ;

//=============================================================================
// Variables
//=============================================================================

//=============================================================================
// Functions
//=============================================================================

mat4 getBoneTransform()
{
	mat4 boneTransform = bones[in_IDs[0]] * in_Weights[0];

	boneTransform += bones[in_IDs[1]] * in_Weights[1];
	boneTransform += bones[in_IDs[2]] * in_Weights[2];
	boneTransform += bones[in_IDs[3]] * in_Weights[3];

	return boneTransform;
}

//=============================================================================
// Main
//=============================================================================

void main()
{
	mat4 boneTransform = getBoneTransform();

    // Calculate the normal in world space.
    normal = mat3(transpose(inverse(model * boneTransform))) * in_Normal;

    // Calculate the tangent in world space.
    tangent = mat3(transpose(inverse(model * boneTransform))) * in_Tangents;

    // Calculate the bitangent in world space.
    bitangent = mat3(transpose(inverse(model * boneTransform))) * in_Bitangents;

    // Calculate the screen-space position of the vertex.
	gl_Position = mvp * boneTransform * vec4(in_Position, 1.0);

    // Calculate the world-space position of the vertex.
	fragPos = vec3(model * boneTransform * vec4(in_Position, 1.0));

    // Pass the texture coordinates.
	texCoords = in_TexCoord;

	// Calculate all light space position for each light and cascade index.
	for(int i = 0; i < NUM_CASCADES * MAX_LIGHTS; ++i)
	{
		lightSpacePos[i] = lightMVP[i] * vec4(in_Position, 1.0);
	}

	// Calculate the object's Z position in the clip space.
	clipSpaceZ = gl_Position.z;
}

//=============================================================================
// End of file
//=============================================================================