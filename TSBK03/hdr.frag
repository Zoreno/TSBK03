#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D hdrBuffer;
uniform sampler2D bloomBuffer;

uniform bool hdr;
uniform bool bloom;

uniform float exposure;
uniform float gamma;

uniform bool fxaa = true;
uniform bool showEdges = false;
uniform vec2 texelStep;
uniform float lumaThreshold = 1.0/8.0;
uniform float mulReduce = 1.0/8.0;
uniform float minReduce = 1.0/128.0;
uniform float maxSpan = 8.0;

uniform sampler2D godrayTexture;
uniform float godrayDensity;
uniform float godrayWeight;
uniform float godrayDecay;
uniform float godrayExposure;
uniform float godrayNumSamples;
uniform vec2 screenSpaceLightPos;

uniform float cameraFacingLightFactor;

vec3 calculateLightScattering(ivec2 offset)
{
	vec3 fragColor = vec3(0.0, 0.0, 0.0);

	vec2 deltaTexCoord = vec2(TexCoords - screenSpaceLightPos);

	vec2 uv = TexCoords;
	deltaTexCoord *= (1.0 / godrayNumSamples) * godrayDensity;
	float illuminationDecay = 1.0;

	for(float i = 0; i < godrayNumSamples; i += 1.0)
	{
		uv -= deltaTexCoord;
		vec3 samp = textureOffset(godrayTexture, uv, offset).xyz;
		samp *= illuminationDecay * godrayWeight;
		fragColor += samp;
		illuminationDecay *= godrayDecay;
	}

	fragColor *= godrayExposure;

	return fragColor * cameraFacingLightFactor;
}

vec3 FXAASampleTexture(vec2 texCoords)
{
	vec3 c = texture(hdrBuffer, texCoords).rgb;

	if(bloom)
		c += texture(bloomBuffer, texCoords).rgb;
	

	c += calculateLightScattering(ivec2(0, 0));
//=============================================================================
// Simple Tone Mapping
//=============================================================================

	if(hdr)
	{
		c = vec3(1.0) - exp(-c * exposure);
		c = pow(c, vec3(1.0/gamma));
	}
	else
	{
		c = clamp(c ,0.0, 1.0);
	}

	return c;
}

vec3 FXAASampleTextureOffset(vec2 texCoords, ivec2 offset)
{
	vec3 c = textureOffset(hdrBuffer, texCoords, offset).rgb;

	if(bloom)
		c += textureOffset(bloomBuffer, texCoords, offset).rgb;

	c += calculateLightScattering(offset);
//=============================================================================
// Simple Tone Mapping
//=============================================================================

	if(hdr)
	{
		c = vec3(1.0) - exp(-c * exposure);
		c = pow(c, vec3(1.0/gamma));
	}
	else
	{
		c = clamp(c ,0.0, 1.0);
	}

	return c;
}

void main()
{

//=============================================================================
// Sample texture
//=============================================================================

	vec3 rgbM = FXAASampleTexture(TexCoords);

	// The result vector is now mapped to [0, 1]

	if(!fxaa)
	{
		color = vec4(rgbM, 1.0f);

		return;
	}

//=============================================================================
// Sample around the midpoint
//=============================================================================

	vec3 rgbNW = FXAASampleTextureOffset(TexCoords, ivec2(-1, 1));
	vec3 rgbNE = FXAASampleTextureOffset(TexCoords, ivec2(1, 1));
	vec3 rgbSW = FXAASampleTextureOffset(TexCoords, ivec2(-1, -1));
	vec3 rgbSE = FXAASampleTextureOffset(TexCoords, ivec2(1, -1));

//=============================================================================
// Convert to luma representation
//=============================================================================

	const vec3 toLuma = vec3(0.299, 0.587, 0.144);

	float lumaNW = dot(rgbNW, toLuma);
	float lumaNE = dot(rgbNE, toLuma);
	float lumaSW = dot(rgbSW, toLuma);
	float lumaSE = dot(rgbSE, toLuma);
	float lumaM = dot(rgbM, toLuma);

	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

	if(lumaMax - lumaMin <= lumaMax * lumaThreshold)
	{
		color = vec4(rgbM, 1.0);

		return;
	}

//=============================================================================
// Calculate the sampling direction
//=============================================================================

	vec2 samplingDirection;
	samplingDirection.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	samplingDirection.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

	float samplingDirectionReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * mulReduce, minReduce);

	float minSamplingDirectionFactor = 1.0 / (min(abs(samplingDirection.x), abs(samplingDirection.y)) + samplingDirectionReduce);

	samplingDirection = clamp(samplingDirection * minSamplingDirectionFactor, vec2(-maxSpan), vec2(maxSpan)) * texelStep;

//=============================================================================
// Sample in direction
//=============================================================================

	vec3 rgbSampleNeg = FXAASampleTexture(TexCoords + samplingDirection * (1.0/3.0 - 0.5));
	vec3 rgbSamplePos = FXAASampleTexture(TexCoords + samplingDirection * (2.0/3.0 - 0.5));

	vec3 rgbTwoTab = (rgbSamplePos + rgbSampleNeg) * 0.5;

	vec3 rgbSampleNegOuter = FXAASampleTexture(TexCoords + samplingDirection * (0.0/3.0 - 0.5));
	vec3 rgbSamplePosOuter = FXAASampleTexture(TexCoords + samplingDirection * (3.0/3.0 - 0.5));

	vec3 rgbFourTab = (rgbSamplePosOuter + rgbSampleNegOuter) * 0.25 + rgbTwoTab * 0.5;

	float lumaFourTab = dot(rgbFourTab, toLuma);
	
//=============================================================================
// Output result
//=============================================================================

	if(lumaFourTab < lumaMin || lumaFourTab > lumaMax)
	{
		color = vec4(rgbTwoTab, 1.0);
	}
	else
	{
		color = vec4(rgbFourTab, 1.0);
	}

	if(showEdges)
	{
		color.r = 1.0;
	}
}

//=============================================================================
// End of file.
//=============================================================================