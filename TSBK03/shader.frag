//=============================================================================
// Fragment Shader
//=============================================================================

#version 430 core

//=============================================================================
// Types
//=============================================================================

#define MAX_LIGHTS 8
#define NUM_CASCADES 3

struct PointLight
{
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
};

struct DirectionalLight
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 direction;
};

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float exponent;
};

//=============================================================================
// Input
//=============================================================================

in vec3 normal;
in vec3 tangent;
in vec3 bitangent;
in vec3 fragPos;
in vec2 texCoords;

in vec4 lightSpacePos[NUM_CASCADES * MAX_LIGHTS];
in float clipSpaceZ;

flat in float terrainHeight;

//=============================================================================
// Uniforms
//=============================================================================

uniform Material material;

uniform vec4 color;
uniform sampler2D texUnit;

uniform int numPointLights;
uniform PointLight pointLight[MAX_LIGHTS];

uniform int numDirectionalLights;
uniform DirectionalLight directionalLight[MAX_LIGHTS];

uniform vec3 viewPos;

uniform vec3 celThresholds;

uniform bool useTexture = true;

uniform float CSMEndClipSpace[NUM_CASCADES + 1];

uniform sampler2DShadow shadowMap[NUM_CASCADES * MAX_LIGHTS];

uniform bool terrain = false;

uniform bool snow;

uniform vec3 tintColor;

//=============================================================================
// Outputs
//=============================================================================

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

//=============================================================================
// Variables
//=============================================================================

//=============================================================================
// Functions
//=============================================================================

float calculateShadowFactor(int lightIndex, int cascadeIndex, vec4 lightSpacePos)
{
	// Find the UV coordinates to sample and the depth to compare with.
	vec3 projectionCoordinates = lightSpacePos.xyz / lightSpacePos.w;
	vec3 UVCoords = 0.5 * projectionCoordinates.xyz + 0.5;

	// Bias the depth a little
	UVCoords.z -= 0.005;

	// Sample the shadow map
	float depth = texture(shadowMap[NUM_CASCADES * lightIndex + cascadeIndex], UVCoords.xyz).r;

	return depth;
}

float stepmix(float edge0, float edge1, float E, float x)
{
	float T = clamp(0.5 * (x - edge0 + E) / E, 0.0, 1.0);

	return mix(edge0, edge1, T);
}

vec2 calculateCelIntensity(vec3 lightDirection, vec3 normal)
{
	vec2 result;

	vec3 N = normalize(normal);
	vec3 L = normalize(lightDirection);
		
	float df = max(0.0, dot(N, L));

	const float A = celThresholds.x;
	const float B = celThresholds.y;
	const float C = celThresholds.z;
	const float D = 1.0;
	float E = fwidth(df);

	if(df > A - E && df < A + E)
	{
		df = stepmix(A, B, E, df);
	}
	else if(df > B - E && df < B + E)
	{
		df = stepmix(B, C, E, df);
	}
	else if(df > C - E && df < C + E)
	{
		df = stepmix(B, C, E, df);
	}
	else if(df < A)
	{
		df = A;
	}
	else if(df < B)
	{
		df = B;
	}
	else if(df < C)
	{
		df = C;
	}
	else
	{
		df = D;
	}
	
	return vec2(df, df * 0.8);
}

float calculateAttenuation(float distance, PointLight light)
{
	return 1.f/(light.constant + distance * (light.linear + distance * light.quadratic));
}

vec3 calculatePointLights(Material mat)
{
	// Make sure that the normal is normalized
	vec3 norm = normalize(normal);

	// Calculate the vector from the camera to the fragment position
	vec3 viewDir = normalize(viewPos - fragPos);

	// Allocate a color accumulator
	vec3 resultColor = vec3(0, 0, 0);

	for(int i = 0; i < numPointLights; ++i)
	{
		// Calculate the vector from the light to the fragment position
		vec3 lightDirection = normalize(pointLight[i].position - fragPos);

		// Ambient color calculation
		vec3 ambientColor = pointLight[i].ambient * mat.ambient;

		// Diffuse color calculation
		float diffFactor = max(dot(norm, lightDirection), 0.0);
		vec3 diffuseColor = pointLight[i].diffuse * diffFactor * mat.diffuse;

		// Specular color calculation
		vec3 reflectDir = reflect(-lightDirection, norm);
		float specFactor = pow(max(dot(viewDir, reflectDir), 0.0), mat.exponent);
		vec3 specularColor = pointLight[i].specular * specFactor * mat.specular;
		
		// Calculate attenuation
		float lightDistance = length(pointLight[i].position - fragPos);
		float attenuation = calculateAttenuation(lightDistance, pointLight[i]);

		// Cel intensity
		vec2 celIntensity = calculateCelIntensity(lightDirection, norm);

		// Add the color components to the accumulator
		resultColor += (ambientColor + diffuseColor*celIntensity.x + specularColor*celIntensity.y) * attenuation;
	}

	return resultColor;
}

vec3 calculateDirectionalLights(Material mat)
{
	// Make sure that the normal is normalized
	vec3 norm = normalize(normal);

	// Calculate the vector from the camera to the fragment position
	vec3 viewDir = normalize(viewPos - fragPos);

	// Allocate a color accumulator
	vec3 resultColor = vec3(0, 0, 0);

	for(int i = 0; i < numDirectionalLights; ++i)
	{
		// Calculate the vector from the light to the fragment position
		vec3 lightDirection = normalize(-directionalLight[i].direction);

		// Ambient color calculation
		vec3 ambientColor = directionalLight[i].ambient * mat.ambient;

		// Diffuse color calculation
		float diffFactor = max(dot(norm, lightDirection), 0.0);
		vec3 diffuseColor = directionalLight[i].diffuse * diffFactor * mat.diffuse;

		// Specular color calculation
		vec3 reflectDir = reflect(-lightDirection, norm);
		float specFactor = pow(max(dot(viewDir, reflectDir), 0.0), mat.exponent);
		vec3 specularColor = directionalLight[i].specular * specFactor * mat.specular;

		// Cel intensity
		vec2 celIntensity = calculateCelIntensity(lightDirection, norm);

		float shadowFactor = 0.0;
		vec3 cascadeIndicator = vec3(0.0, 0.0, 0.0);

		for(int cascadeIndex = 0; cascadeIndex < NUM_CASCADES; ++cascadeIndex)
		{
			if(clipSpaceZ <= CSMEndClipSpace[cascadeIndex])
			{
				shadowFactor = calculateShadowFactor(i, cascadeIndex, lightSpacePos[i * NUM_CASCADES + cascadeIndex]);
			
				if (cascadeIndex == 0) 
					cascadeIndicator = vec3(0.1, 0.0, 0.0);
				else if (cascadeIndex == 1)
					cascadeIndicator = vec3(0.0, 0.1, 0.0);
				else if (cascadeIndex == 2)
					cascadeIndicator = vec3(0.0, 0.0, 0.1);
				break;
			}
		}

		//resultColor += vec3(clipSpaceZ) / 100.f;

		// Add the color components to the accumulator
		// resultColor += vec3(shadowFactor) - cascadeIndicator;
		resultColor += (ambientColor + (0.5 + shadowFactor * 0.5) * (diffuseColor*celIntensity.x + specularColor*celIntensity.y));
	}

	return resultColor;
}

vec3 calculateLight(Material mat)
{
	vec3 resultColor = vec3(0.0, 0.0, 0.0);

	resultColor += calculatePointLights(mat);
	resultColor += calculateDirectionalLights(mat);

	return resultColor;
}

//=============================================================================
// Main
//=============================================================================

void main()
{
	vec3 objColor;

	Material mat;

    if(useTexture)
	{
		objColor = texture(texUnit, texCoords).rgb;

		mat.ambient = vec3(1.0, 1.0, 1.0);
		mat.diffuse = vec3(1.0, 1.0, 1.0);
		mat.specular = vec3(1.0, 1.0, 1.0);
		mat.exponent = 64;
	}
	else if(!terrain)
	{
		objColor = color.xyz;

		mat = material;

		mat.exponent = 8;
	}
	else
	{
		objColor = color.xyz;

		float height = terrainHeight;

		if(height > -1 && height < 12)
		{
			mat.ambient = vec3(0.1, 0.1, 0.1);
			mat.diffuse = vec3(0.93, 0.78, 0.68);
			mat.specular = vec3(0.1, 0.1, 0.1);
			mat.exponent = 4;
		}
		else if(height >= 12 && height < 22)
		{
			mat.ambient = vec3(0.1, 0.1, 0.1);
			mat.diffuse = vec3(1.0, 166.0, 17.0)  / 256.0;
			mat.specular = vec3(0.1, 0.1, 0.1);
			mat.exponent = 4;
		}
		else if(height >= 22 && height < 35)
		{
			mat.ambient = vec3(0.1, 0.1, 0.1);
			mat.diffuse = vec3(185.0, 195.0, 205.0) / 256.0;
			mat.specular = vec3(0.1, 0.1, 0.1);
			mat.exponent = 4;
		}
		else if(height >= 35 && height < 60)
		{
			mat.ambient = vec3(0.1, 0.1, 0.1);
			mat.diffuse = vec3(1.0, 1.0, 1.0) * 1.2;
			mat.specular = vec3(0.1, 0.1, 0.1);
			mat.exponent = 4;
		}
		else
		{
			mat = material;
		}
	}

	vec3 norm = normalize(normal);
	vec3 up = vec3(0.0, 1.0, 0.0);

	// Snow

	if(snow)
	{
		mat.ambient = mix(mat.ambient, vec3(1.0, 1.0, 1.0), smoothstep(0.0, 0.2, max(0.0, dot(norm, up))));
		mat.diffuse = mix(mat.diffuse, vec3(1.0, 1.0, 1.0), smoothstep(0.0, 0.2, max(0.0, dot(norm, up))));
	}

	vec3 lightColor = calculateLight(mat);

	vec3 outputColor = objColor * lightColor;

	outputColor += tintColor;

    FragColor = vec4(outputColor, 1.0);

	float brightness = dot(outputColor, vec3(0.2126,0.7152,0.0722));

	if(brightness > 1.0)
	{
		BrightColor = vec4(outputColor, 1.0);
	}
	else
	{
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}

//=============================================================================
// End of file
//=============================================================================