#include "Renderer.h"
#include "PointLightSceneNode.h"
#include "DirectionalLightSceneNode.h"

#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "StaticModelSceneNode.h"
#include "TerrainSceneNode.h"
#include "Terrain.h"

static GLfloat quadVertices[] = {
	// Positions			// Texture Coords
	-1.0f, 1.0f, 0.0f,		0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
	1.0f, 1.0f, 0.0f,		1.0f, 1.0f,
	1.0f, 1.0f, 0.0f,		1.0f, 1.0f,
	-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
	1.0f, -1.0f, 0.0f,		1.0f, 0.0f,
};

static GLfloat boxVertices[] =
{
	// From				To
	// Bottom plane
	-0.5f, -0.5f, -0.5f,	-0.5f, -0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f,		0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,		0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,		-0.5f, -0.5f, -0.5f,

	// Poles
	-0.5f, -0.5f, -0.5f,	-0.5f, 0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,	-0.5f, 0.5f, 0.5f,
	0.5f, -0.5f, 0.5f, 		0.5f, 0.5f, 0.5f,
	0.5f, -0.5f, -0.5f,		0.5f, 0.5f, -0.5f,

	// Top plane
	-0.5f, 0.5f, -0.5f,		-0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f,		0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,		0.5f, 0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,		-0.5f, 0.5f, -0.5f,
};

Renderer::Renderer(
	int windowWidth,
	int windowHeight,
	AssetManager *assetManager,
	glm::mat4 projection)
	: _windowWidth{ windowWidth },
	_windowHeight{ windowHeight },
	_assetManager{ assetManager },
	_projection{ projection }
{
	//=========================================================================
	// Setup Shaders
	//=========================================================================

	_shader.setVertexShaderSource("shader.vert");
	_shader.setFragmentShaderSource("shader.frag");

	_skinnedShader.setVertexShaderSource("skinnedshader.vert");
	_skinnedShader.setFragmentShaderSource("shader.frag");

	_blurShader.setVertexShaderSource("blur.vert");
	_blurShader.setFragmentShaderSource("blur.frag");

	_hdrShader.setVertexShaderSource("hdr.vert");
	_hdrShader.setFragmentShaderSource("hdr.frag");

	_pickingShader.setVertexShaderSource("picking.vert");
	_pickingShader.setFragmentShaderSource("picking.frag");

	_skyboxShader.setVertexShaderSource("skybox.vert");
	_skyboxShader.setFragmentShaderSource("skybox.frag");

	_outlinesBoxShader.setVertexShaderSource("outlinesBox.vert");
	_outlinesBoxShader.setFragmentShaderSource("outlinesBox.frag");

	_csmShader.setVertexShaderSource("csm.vert");
	_csmShader.setFragmentShaderSource("csm.frag");

	_godrayOcclusionShader.setVertexShaderSource("godrayOcclusion.vert");
	_godrayOcclusionShader.setFragmentShaderSource("godrayOcclusion.frag");

	_waterShader.setVertexShaderSource("water.vert");
	_waterShader.setFragmentShaderSource("water.frag");
	_waterShader.setTessellationControlSource("water.tctl");
	_waterShader.setTessellationEvaluationSource("water.tevl");

	try
	{
		_shader.compile();
		_skinnedShader.compile();
		_blurShader.compile();
		_hdrShader.compile();
		_pickingShader.compile();
		_skyboxShader.compile();
		_outlinesBoxShader.compile();
		_csmShader.compile();
		_godrayOcclusionShader.compile();
		_waterShader.compile();
	}
	catch (const GLSLShaderCompilationException& ex)
	{
		std::cerr << ex.what() << std::endl;
		exit(1);
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glEnable(GL_DEPTH_CLAMP);

	//=========================================================================
	// Setup performance query
	//=========================================================================

	glGenQueries(2, _queryID);

	//=========================================================================
	// Setup Screen Quad
	//=========================================================================

	glGenVertexArrays(1, &_quadVAO);
	glGenBuffers(1, &_quadVBO);

	glBindVertexArray(_quadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, _quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		5 * sizeof(GLfloat),
		static_cast<GLvoid *>(nullptr)
	);

	glEnableVertexAttribArray(1);

	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		5 * sizeof(GLfloat),
		reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat))
	);

	glBindVertexArray(0);

	//=========================================================================
	// Setup Lines box
	//=========================================================================

	glGenVertexArrays(1, &_boxVAO);
	glGenBuffers(1, &_boxVBO);

	glBindVertexArray(_boxVAO);

	glBindBuffer(GL_ARRAY_BUFFER, _boxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		3 * sizeof(GLfloat),
		static_cast<GLvoid *>(nullptr)
	);

	glBindVertexArray(0);

	//=========================================================================
	// Setup the Cascaded Shadow Map framebuffers and textures
	//=========================================================================

	// TODO: This may be optimized in many ways:
	// 
	// * Use Texture2DArrays instead and use a geometry shader to depermine
	//   which layer is used.

	glGenFramebuffers(MAX_LIGHTS, _csmFBO);

	// This is a lot of video memory
	glGenTextures(MAX_LIGHTS * NUM_CASCADES, _csmTextures);

	for (unsigned int i = 0; i < MAX_LIGHTS; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _csmFBO[i]);

		for (unsigned int j = 0; j < NUM_CASCADES; ++j)
		{
			glBindTexture(GL_TEXTURE_2D, _csmTextures[i * NUM_CASCADES + j]);

			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_DEPTH_COMPONENT32,
				_shadowSize,
				_shadowSize,
				0,
				GL_DEPTH_COMPONENT,
				GL_FLOAT,
				nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

		}

		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D,
			_csmTextures[NUM_CASCADES * i],
			0);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "CSM Framebuffer not complete!" << std::endl;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

	_cascadeEnds[0] = -0.1f; // This must be equal to zNear
	_cascadeEnds[1] = -25.0f;
	_cascadeEnds[2] = -90.0f;
	_cascadeEnds[3] = -200.f; // This must be equal to zFar

	for (unsigned int i = 0; i < NUM_CASCADES * MAX_LIGHTS; ++i)
	{
		_orthoProjections[i] = glm::mat4{ 1.f };
	}

	//=========================================================================
	// Setup the Godray Occlusion
	//=========================================================================

	glGenFramebuffers(1, &_godrayFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _godrayFBO);

	glGenTextures(1, &_godrayTexture);

	glBindTexture(GL_TEXTURE_2D, _godrayTexture);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		_godrayOcclusionSize,
		_godrayOcclusionSize,
		0,
		GL_RGBA,
		GL_UNSIGNED_INT,
		nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		_godrayTexture,
		0);

	glGenRenderbuffers(1, &_godrayDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, _godrayDepth);

	glRenderbufferStorage(
		GL_RENDERBUFFER,
		GL_DEPTH_COMPONENT,
		_godrayOcclusionSize,
		_godrayOcclusionSize);

	glFramebufferRenderbuffer(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER,
		_godrayDepth);

	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Godray Framebuffer not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	constructWindowSizeDependentObjects();

	GLint MaxPatchVertices = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
	printf("Max supported patch vertices %d\n", MaxPatchVertices);
	glPatchParameteri(GL_PATCH_VERTICES, 3);
}

Renderer::~Renderer()
{
	// TODO: Remove all the framebuffers
	destroyWindowSizeDependentObjects();
}

void Renderer::constructWindowSizeDependentObjects()
{
	//=========================================================================
	// Setup HDR Framebuffer
	//=========================================================================

	glGenFramebuffers(1, &_hdrFBO);
	glGenTextures(2, _colorBuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, _hdrFBO);

	for (int i = 0; i < 2; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, _colorBuffers[i]);

		// Generate the texture as a floating point buffer
		// of screen size.
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA16F,
			_windowWidth,
			_windowHeight,
			0,
			GL_RGBA,
			GL_FLOAT,
			nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Set the texture as color buffer i
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0 + i,
			GL_TEXTURE_2D,
			_colorBuffers[i],
			0);
	}

	glGenRenderbuffers(1, &_rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, _rboDepth);

	glRenderbufferStorage(
		GL_RENDERBUFFER,
		GL_DEPTH_COMPONENT,
		_windowWidth,
		_windowHeight);

	glFramebufferRenderbuffer(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER,
		_rboDepth);

	GLuint attachments[2] =
	{
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
	};

	glDrawBuffers(2, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "HDR Framebuffer not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//=========================================================================
	// Setup Bloom Framebuffer
	//=========================================================================

	glGenFramebuffers(2, _pingpongFBO);
	glGenTextures(2, _pingpongColorbuffers);

	for (int i = 0; i < 2; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, _pingpongColorbuffers[i]);

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGB16F,
			_windowWidth,
			_windowHeight,
			0,
			GL_RGB,
			GL_FLOAT,
			nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D,
			_pingpongColorbuffers[i],
			0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "Bloom Framebuffer " << i << " not complete!" << std::endl;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//=========================================================================
	// Setup the picking texture
	//=========================================================================

	glGenFramebuffers(1, &_pickingFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _pickingFBO);

	glGenTextures(1, &_pickingTexture);
	glBindTexture(GL_TEXTURE_2D, _pickingTexture);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB32F,
		_windowWidth,
		_windowHeight,
		0,
		GL_RGB,
		GL_FLOAT,
		nullptr);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		_pickingTexture,
		0);

	glGenTextures(1, &_pickingDepth);
	glBindTexture(GL_TEXTURE_2D, _pickingDepth);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_DEPTH_COMPONENT,
		_windowWidth,
		_windowHeight,
		0,
		GL_DEPTH_COMPONENT,
		GL_FLOAT,
		nullptr);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D,
		_pickingDepth,
		0);

	glReadBuffer(GL_NONE);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Picking Framebuffer not complete!" << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::destroyWindowSizeDependentObjects()
{
	glDeleteFramebuffers(1, &_hdrFBO);
	glDeleteTextures(2, _colorBuffers);
	glDeleteRenderbuffers(1, &_rboDepth);

	glDeleteFramebuffers(2, _pingpongFBO);
	glDeleteTextures(2, _pingpongColorbuffers);

	glDeleteFramebuffers(1, &_pickingFBO);
	glDeleteTextures(1, &_pickingTexture);

}

void Renderer::calculateOrthoProjections(int lightIndex)
{
	float FOV = 70.f; // TODO: This should be the FOV used in the projection.

	glm::vec3 lightDirection = glm::normalize(_directionalLights[lightIndex].first.getDirection());

	// The NDC cube corners
	glm::vec4 frustumCorners[8] =
	{
		glm::vec4{ 1.f, 1.f, -1.f, 1.f },
		glm::vec4{ 1.f, -1.f, -1.f, 1.f },
		glm::vec4{ -1.f, 1.f, -1.f, 1.f },
		glm::vec4{ -1.f, -1.f, -1.f, 1.f },

		glm::vec4{ 1.f, 1.f, 1.f, 1.f },
		glm::vec4{ 1.f, -1.f, 1.f, 1.f },
		glm::vec4{ -1.f, 1.f, 1.f, 1.f },
		glm::vec4{ -1.f, -1.f, 1.f, 1.f },
	};

	for (unsigned int i = 0; i < NUM_CASCADES; ++i)
	{
		// Vectors for the 8 frustum corners in world space
		glm::vec3 frustumCornersWorldSpace[8];

		// The center of the frustum
		glm::vec3 frustumCenter = glm::vec3{ 0.f };

		for (unsigned int j = 0; j < 8; ++j)
		{
			// Create a projection which is a portion of the
			// camera viewing frustum for a given split.
			glm::mat4 projection = glm::perspective(
				glm::radians(FOV),	// FOV
				static_cast<float>(_windowWidth) / static_cast<float>(_windowHeight), // Aspect
				-_cascadeEnds[i], // zNear
				-_cascadeEnds[i + 1]); // zFar

			// Apply the inverse of the splitted projection to the corners
			// to get the corners of the NDC cube given in camera space.
			glm::vec4 cornerCameraSpace = glm::inverse(projection) * frustumCorners[j];

			// Apply the inverse of the camera matrix to get the corners in
			// in world space.
			glm::vec4 cornerWorldSpace = glm::inverse(_cameraTransform) * cornerCameraSpace;

			// Perform the perspective divide.
			cornerWorldSpace /= cornerWorldSpace.w;

			// Save the world space frustum corner for later calculations
			frustumCornersWorldSpace[j] = cornerWorldSpace;

			// Add the corner to the frustum center accumulator.
			frustumCenter += frustumCornersWorldSpace[j];
		}

		// Divide the center with the amount of corners added to the
		// accumulator to get the actual center (vector mean)
		frustumCenter /= 8.f;

		// Create a variable to calculate the smallest radius to contain all
		// frustum corners
		float radius = 0.f;

		for (unsigned int j = 0; j < 8; ++j)
		{
			// Calculate the distance from the center to the corner of the 
			// frustum
			float distance = glm::length(frustumCornersWorldSpace[j] - frustumCenter);

			// Save the distance if the distance was larger than the largest
			// we have had so far.
			radius = glm::max(radius, distance);
		}

		// Round the radius to the next 16th of an unit.
		radius = std::ceil(radius * 16.f) / 16.f;

		// Create a max and min extents AABB that contains the radius
		// in all directions.
		glm::vec3 maxExtents = glm::vec3{ radius, radius, radius };
		glm::vec3 minExtents = -maxExtents;

		// Calculate the light direction from the frustum center
		glm::vec3 lightDir = frustumCenter - glm::normalize(lightDirection) * -minExtents.z;

		// Construct the light view matrix looking from the light direction at
		// the center of the frustum
		glm::mat4 lightViewMatrix = glm::lookAt(lightDir, frustumCenter, glm::vec3{ 0.0f, 1.0f, 0.0f });

		// Calculate the total cascade frustum extents
		glm::vec3 cascadeExtents = maxExtents - minExtents;

		// Store the light view matrix
		_lightViewMatrices[lightIndex * NUM_CASCADES + i] = lightViewMatrix;

		glm::mat4 orthoProj = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.f, cascadeExtents.z);

		glm::mat4 shadowMatrix = orthoProj * lightViewMatrix;
		glm::vec4 shadowOrigin = glm::vec4{ 0.f, 0.f, 0.f, 1.f };

		shadowOrigin = shadowMatrix * shadowOrigin;
		shadowOrigin = shadowOrigin * static_cast<float>(_shadowSize) / 2.f;

		glm::vec4 roundedOrigin = glm::round(shadowOrigin);
		glm::vec4 roundOffset = roundedOrigin - shadowOrigin;

		roundOffset = roundOffset * 2.f / static_cast<float>(_shadowSize);
		roundOffset.z = 0.f;
		roundOffset.w = 0.f;

		orthoProj[3] += roundOffset;

		// Store the light projection
		_orthoProjections[lightIndex *NUM_CASCADES + i] = orthoProj;
	}
}

void Renderer::doCSMShadowPass(
	Scene *scene)
{
	for (unsigned int i = 0; i < _directionalLights.size(); ++i)
	{
		calculateOrthoProjections(i);

		glBindFramebuffer(GL_FRAMEBUFFER, _csmFBO[i]);

		for (unsigned int j = 0; j < NUM_CASCADES; ++j)
		{
			glFramebufferTexture2D(
				GL_FRAMEBUFFER,
				GL_DEPTH_ATTACHMENT,
				GL_TEXTURE_2D,
				_csmTextures[i * NUM_CASCADES + j],
				0);

			glClear(GL_DEPTH_BUFFER_BIT);

			_currentCascade = i * NUM_CASCADES + j;

			renderScene(scene, 2);
		}
	}
}

void Renderer::render(
	Scene *scene)
{
	unsigned long long int startTime;
	unsigned long long int stopTime;

	// Extract Lights
	_pointLights.clear();
	_directionalLights.clear();

	glQueryCounter(_queryID[0], GL_TIMESTAMP);

	// TODO: This should only be done when the scene has changed.
	extractLights(scene);

	//std::cout << pointLights.size() << std::endl;
	//std::cout << directionalLights.size() << std::endl;

	// Do Render Pass

	doPickingRenderPass(scene);

	if (_enableGodrays)
	{
		doGodrayOcclusionRenderingPass(scene);
	}

	doCSMShadowPass(scene);

	doColorRenderingPass(scene);

	if (_bloom)
	{
		doBloomBlurRenderingPass();
	}

	doScreenRenderPass();

	// Timing stuff

	glQueryCounter(_queryID[1], GL_TIMESTAMP);

	int stopTimerAvailable = 0;
	while (!stopTimerAvailable)
	{
		glGetQueryObjectiv(
			_queryID[1],
			GL_QUERY_RESULT_AVAILABLE,
			&stopTimerAvailable);
	}

	glGetQueryObjectui64v(_queryID[0], GL_QUERY_RESULT, &startTime);
	glGetQueryObjectui64v(_queryID[1], GL_QUERY_RESULT, &stopTime);

	_lastRenderTime = (stopTime - startTime) / 1000000.f;
}

void Renderer::setCameraTransform(
	const glm::mat4 &cameraTransform)
{
	_cameraTransform = cameraTransform;
}

void Renderer::setProjection(
	const glm::mat4 &projection)
{
	_projection = projection;
}

void Renderer::setCameraPosition(
	const glm::vec3 &cameraPos)
{
	_cameraPosition = cameraPos;
}

void Renderer::setCameraDirection(
	const glm::vec3 &newDirection)
{
	_cameraDirection = newDirection;
}

float Renderer::getLastGPURenderTime() const
{
	return _lastRenderTime;
}

void Renderer::changeResolution(
	int newWidth,
	int newHeight)
{
	destroyWindowSizeDependentObjects();

	_windowWidth = newWidth;
	_windowHeight = newHeight;

	constructWindowSizeDependentObjects();
}

int Renderer::getWindowWidth() const
{
	return _windowWidth;
}

int Renderer::getWindowHeight() const
{
	return _windowHeight;
}

bool Renderer::getHdr() const
{
	return _hdr;
}

bool Renderer::getBloom() const
{
	return _bloom;
}

float Renderer::getExposure() const
{
	return _exposure;
}

float Renderer::getGamma() const
{
	return _gamma;
}

void Renderer::setHdr(
	bool hdr)
{
	_hdr = hdr;
}

void Renderer::setBloom(
	bool bloom)
{
	_bloom = bloom;
}

void Renderer::setExposure(
	float exposure)
{
	_exposure = exposure;
}

void Renderer::setGamma(
	float gamma)
{
	_gamma = gamma;
}

RendererPickingInfo Renderer::getPickingInfo(
	int x,
	int y)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _pickingFBO);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	RendererPickingInfo pixel;
	glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, &pixel);

	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	return pixel;
}

bool Renderer::getFXAA() const
{
	return _fxaa;
}

bool Renderer::getShowEdges() const
{
	return _showEdges;
}

float Renderer::getLumaThreshold() const
{
	return _lumaThreshold;
}

float Renderer::getMulReduce() const
{
	return _mulReduce;
}

float Renderer::getMinReduce() const
{
	return _minReduce;
}

float Renderer::getMaxSpan() const
{
	return _maxSpan;
}

void Renderer::setFXAA(
	bool fxaa)
{
	_fxaa = fxaa;
}

void Renderer::setShowEdges(
	bool showEdges)
{
	_showEdges = showEdges;
}

void Renderer::setLumaThreshold(
	float lumaThreshold)
{
	_lumaThreshold = lumaThreshold;
}

void Renderer::setMulReduce(
	float mulReduce)
{
	_mulReduce = mulReduce;
}

void Renderer::setMinReduce(
	float minReduce)
{
	_minReduce = minReduce;
}

void Renderer::setMaxSpan(
	float maxSpan)
{
	_maxSpan = maxSpan;
}

void Renderer::setSkybox(
	const Skybox &skybox)
{
	_skybox = skybox;
}

const GLuint * Renderer::getCSMTextures() const
{
	return _csmTextures;
}

GLuint Renderer::getGodrayOcclusionTexture() const
{
	return _godrayTexture;
}

float Renderer::getGodrayDensity() const
{
	return _godrayDensity;
}

float Renderer::getGodrayWeight() const
{
	return _godrayWeight;
}

float Renderer::getGodrayDecay() const
{
	return _godrayDecay;
}

float Renderer::getGodrayExposure() const
{
	return _godrayExposure;
}

float Renderer::getGodrayNumSamples() const
{
	return _godrayNumSamples;
}

void Renderer::setGodrayDensity(
	float newDensity)
{
	_godrayDensity = newDensity;
}

void Renderer::setGodrayWeight(
	float newWeight)
{
	_godrayWeight = newWeight;
}

void Renderer::setGodrayDecay(
	float newDecay)
{
	_godrayDecay = newDecay;
}

void Renderer::setGodrayExposure(
	float newExposure)
{
	_godrayExposure = newExposure;
}

void Renderer::setGodrayNumSamples(
	float newNumSamples)
{
	_godrayNumSamples = newNumSamples;
}

bool Renderer::getEnableGodrays() const
{
	return _enableGodrays;
}

void Renderer::setEnableGodrays(
	bool newEnableGodrays)
{
	_enableGodrays = newEnableGodrays;
}

void Renderer::extractLights(
	const SceneNode *node)
{
	if (node->getSceneNodeType() == SceneNodeType::POINT_LIGHT)
	{
		// TODO: Insert possible culling of point lights depending on the camera position.

		const PointLightSceneNode *pointLightSceneNode = reinterpret_cast<const PointLightSceneNode *>(node);

		_pointLights.emplace_back(
			pointLightSceneNode->getPointLight(),
			pointLightSceneNode->getPosition());
	}
	else if (node->getSceneNodeType() == SceneNodeType::DIRECTIONAL_LIGHT)
	{
		const DirectionalLightSceneNode *directionalLightSceneNode = reinterpret_cast<const DirectionalLightSceneNode *>(node);

		_directionalLights.emplace_back(
			directionalLightSceneNode->getDirectionalLight(),
			directionalLightSceneNode->getPosition());
	}

	for (auto it : node->getChildren())
	{
		extractLights(it);
	}
}

void Renderer::renderSkybox()
{
	_skyboxShader.use();

	glm::mat4 skyboxModelMatrix = glm::translate(glm::mat4{ 1.f }, _cameraPosition);
	glm::mat4 skyboxViewMatrix = _cameraTransform;

	glm::mat4 skyboxMVP = _projection * skyboxViewMatrix * skyboxModelMatrix;

	Model *model = _assetManager->fetch<Model>(_skybox.getModel());
	TextureCubeMap *texture = _assetManager->fetch<TextureCubeMap>(_skybox.getTexture());

	_skyboxShader.uploadUniform("mvp", skyboxMVP);
	_skyboxShader.uploadUniform("texUnit", 0);

	texture->bind(0);

	GLint OldCullFaceMode;
	glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);
	GLint OldDepthFuncMode;
	glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);

	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);

	for (unsigned int i = 0; i < model->getMeshes().size(); ++i)
	{
		model->getMeshes().at(i)->render();
	}

	glCullFace(OldCullFaceMode);
	glDepthFunc(OldDepthFuncMode);

}

void Renderer::doGodrayOcclusionRenderingPass(
	const SceneNode *scene)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _godrayFBO);

	glClearColor(0.0f, 0.0f, 0.0f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, _godrayOcclusionSize, _godrayOcclusionSize);

	renderScene(scene, 4);
	renderScene(scene, 3);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::doColorRenderingPass(
	const SceneNode *scene)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _hdrFBO);

	glClearColor(0.0f, 0.0f, 0.0f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, _windowWidth, _windowHeight);

	renderSkybox();

	renderScene(scene, 0);



	_waterShader.use();
	_waterShader.uploadUniform("vp", _projection * _cameraTransform);
	_waterShader.uploadUniform("cameraPos", _cameraPosition);

	std::vector<glm::mat4> models;

	for (unsigned int i = 0; i < 10; ++i)
	{
		for (unsigned int j = 0; j < 10; ++j)
		{
			glm::mat4 modelMatrix{ 1.f };
			//modelMatrix = glm::translate(modelMatrix, glm::vec3{512.f, 10.f, 512.f });
			modelMatrix = glm::translate(modelMatrix, glm::vec3{ 25.f + 50.f*i, 10.f, 25.f + 50.f*j });
			modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.f), glm::vec3{ 1.f, 0.f, 0.f });
			modelMatrix = glm::scale(modelMatrix, glm::vec3{ 25.f, 25.f, 25.f });

			models.push_back(modelMatrix);
		}
	}

	float time = glfwGetTime();

	_waterShader.uploadUniform("time", time);

	for (unsigned int i = 0; i < _directionalLights.size(); ++i)
	{
		_waterShader.uploadUniform(std::string{ "lightDir[" } +std::to_string(i) + std::string{ "]" }, _directionalLights.at(i).first.getDirection());
	}

	_waterShader.uploadUniform("numDirectionalLights", static_cast<int>(_directionalLights.size()));

	_waterShader.uploadUniformArray("models", 100, models);

	if (false)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glBindVertexArray(_quadVAO);
	glDrawArraysInstanced(GL_PATCHES, 0, 6, 100);
	glBindVertexArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::doPickingRenderPass(
	const SceneNode *scene)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _pickingFBO);

	glClearColor(0.0f, 0.0f, 0.0f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, _windowWidth, _windowHeight);

	glDisable(GL_BLEND);

	renderScene(scene, 1);

	glEnable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::doBloomBlurRenderingPass()
{
	_blurShader.use();

	bool horizontal = true;
	bool first_iteration = true;

	GLuint amount = 5;

	for (GLuint i = 0; i < (amount * 2); ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _pingpongFBO[horizontal]);

		glClearColor(0.0f, 0.0f, 0.0f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_blurShader.uploadUniform("horizontal", horizontal);
		_blurShader.uploadUniform("image", 0);

		// If this is the first iteration, we need the bright color buffer from the
		// color rendering pass.
		glActiveTexture(GL_TEXTURE0);

		if (first_iteration)
		{
			glBindTexture(GL_TEXTURE_2D, _colorBuffers[1]);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, _pingpongColorbuffers[!horizontal]);
		}

		// Bind and draw the screen quad
		glBindVertexArray(_quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		horizontal = !horizontal;

		if (first_iteration)
		{
			first_iteration = false;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::doScreenRenderPass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_hdrShader.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _colorBuffers[0]);
	_hdrShader.uploadUniform("hdrBuffer", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _pingpongColorbuffers[0]);
	_hdrShader.uploadUniform("bloomBuffer", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _godrayTexture);
	_hdrShader.uploadUniform("godrayTexture", 2);

	_hdrShader.uploadUniform("hdr", _hdr);
	_hdrShader.uploadUniform("bloom", _bloom);
	_hdrShader.uploadUniform("exposure", _exposure);
	_hdrShader.uploadUniform("gamma", _gamma);

	_hdrShader.uploadUniform("fxaa", _fxaa);
	_hdrShader.uploadUniform("showEdges", _showEdges);

	_hdrShader.uploadUniform("texelStep", glm::vec2(1.f / _windowWidth, 1.f / _windowHeight));

	_hdrShader.uploadUniform("lumaThreshold", _lumaThreshold);
	_hdrShader.uploadUniform("mulReduce", _mulReduce);
	_hdrShader.uploadUniform("minReduce", _minReduce);
	_hdrShader.uploadUniform("maxSpan", _maxSpan);

	_hdrShader.uploadUniform("enableGodrays", _enableGodrays);
	_hdrShader.uploadUniform("godrayDensity", _godrayDensity);
	_hdrShader.uploadUniform("godrayWeight", _godrayWeight);
	_hdrShader.uploadUniform("godrayDecay", _godrayDecay);
	_hdrShader.uploadUniform("godrayExposure", _godrayExposure);
	_hdrShader.uploadUniform("godrayNumSamples", _godrayNumSamples);

	float factor = glm::max(
		glm::dot(
			_cameraDirection,
			glm::normalize(_directionalLights.at(0).first.getDirection())), 0.f);

	factor = glm::pow(factor, 6);

	_hdrShader.uploadUniform("cameraFacingLightFactor", factor);

	glm::vec4 v{ -_directionalLights.at(0).first.getDirection(), 1.f };

	glm::mat4 modelMatrix = glm::mat4{ 1.f };
	modelMatrix = glm::translate(modelMatrix, _cameraPosition);

	glm::mat4 skyboxView = _cameraTransform;

	skyboxView = glm::inverse(skyboxView);

	skyboxView[0][3] = 0.f;
	skyboxView[1][3] = 0.f;
	skyboxView[2][3] = 0.f;

	skyboxView = glm::inverse(skyboxView);

	v = _projection * skyboxView * modelMatrix * v;

	v /= v.w;

	v = 0.5f * v + 0.5f;

	glm::vec2 screenSpaceLightPos{ v.x, v.y };

	_hdrShader.uploadUniform("screenSpaceLightPos", screenSpaceLightPos);

	// Render the image on screen.
	glBindVertexArray(_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Renderer::renderScene(
	const SceneNode *node,
	int pass)
{
	if (node->getSceneNodeType() == SceneNodeType::STATIC_MODEL)
	{
		const StaticModelSceneNode *modelNode = reinterpret_cast<const StaticModelSceneNode *>(node);

		if (pass == 0)
		{
			renderStaticModel(modelNode);
		}
		else if (pass == 1)
		{
			renderStaticModelPicking(modelNode);
		}
		else if (pass == 2)
		{
			renderStaticModelCSM(modelNode);
		}
		else if (pass == 3)
		{
			renderStaticModelGodrayOcclusion(modelNode);
		}
	}
	else if (node->getSceneNodeType() == SceneNodeType::DIRECTIONAL_LIGHT)
	{
		const DirectionalLightSceneNode *lightNode = reinterpret_cast<const DirectionalLightSceneNode *>(node);

		if (pass == 4)
		{
			renderDirectionalLightGodrayOcclusion(lightNode);
		}
	}
	else if (node->getSceneNodeType() == SceneNodeType::TERRAIN)
	{
		const TerrainSceneNode *terrainNode = reinterpret_cast<const TerrainSceneNode *>(node);

		if (pass == 0)
		{
			renderTerrain(terrainNode);
		}
		else if (pass == 1)
		{
			renderTerrainPicking(terrainNode);
		}
		else if (pass == 2)
		{
			renderTerrainCSM(terrainNode);
		}
	}

	for (auto it : node->getChildren())
	{
		renderScene(it, pass);
	}
}

void Renderer::renderStaticModel(
	const StaticModelSceneNode *modelNode)
{
	std::string modelTag = modelNode->getModel();

	Model *model = _assetManager->fetch<Model>(modelTag);

	Frustum frustum{ _projection * _cameraTransform };

	std::vector<glm::vec3> points = model->getExtents().getPoints();

	glm::mat4 modelTransform = modelNode->getTransformationMatrix() * model->getCorrectionTransform();

	for(auto& it : points)
	{
		it = modelTransform * glm::vec4{ it, 1.f };
	}

	if(frustum.boxIntersect(points) < 0)
	{
		return;
	}

	GLSLShader *currentShader = &_shader;

	if (model->isSkinned())
	{
		_skinnedShader.use();
		currentShader = &_skinnedShader;

		std::vector<glm::mat4> transforms;

		float time = glfwGetTime();

		if (modelNode->getCurrentAnimation() == "idle")
		{
			time = 1.52;
		}

		model->getBoneTransforms(time, transforms, modelNode->getCurrentAnimation());

		currentShader->uploadUniformArray("bones", transforms.size(), transforms);
	}
	else
	{
		_shader.use();
	}

	glm::mat4 mvp = _projection * _cameraTransform * modelTransform;

	for (unsigned int i = 0; i < _pointLights.size(); ++i)
	{
		currentShader->uploadUniform("pointLight[" + std::to_string(i) + "].constant", _pointLights.at(i).first.getConstant());
		currentShader->uploadUniform("pointLight[" + std::to_string(i) + "].linear", _pointLights.at(i).first.getLinear());
		currentShader->uploadUniform("pointLight[" + std::to_string(i) + "].quadratic", _pointLights.at(i).first.getQuadratic());
		currentShader->uploadUniform("pointLight[" + std::to_string(i) + "].ambient", _pointLights.at(i).first.getAmbient());
		currentShader->uploadUniform("pointLight[" + std::to_string(i) + "].diffuse", _pointLights.at(i).first.getDiffuse());
		currentShader->uploadUniform("pointLight[" + std::to_string(i) + "].specular", _pointLights.at(i).first.getSpecular());
		currentShader->uploadUniform("pointLight[" + std::to_string(i) + "].position", _pointLights.at(i).second);
	}

	for (unsigned int i = 0; i < _directionalLights.size(); ++i)
	{
		currentShader->uploadUniform("directionalLight[" + std::to_string(i) + "].ambient", _directionalLights.at(i).first.getAmbient());
		currentShader->uploadUniform("directionalLight[" + std::to_string(i) + "].diffuse", _directionalLights.at(i).first.getDiffuse());
		currentShader->uploadUniform("directionalLight[" + std::to_string(i) + "].specular", _directionalLights.at(i).first.getSpecular());
		currentShader->uploadUniform("directionalLight[" + std::to_string(i) + "].direction", _directionalLights.at(i).first.getDirection());

		for (unsigned int j = 0; j < NUM_CASCADES; ++j)
		{
			glm::mat4 lightMVP = _orthoProjections[i * NUM_CASCADES + j] * _lightViewMatrices[i * NUM_CASCADES + j] * modelNode->getTransformationMatrix();

			currentShader->uploadUniform("lightMVP[" + std::to_string(i * NUM_CASCADES + j) + "]", lightMVP);
		}
	}

	const unsigned int CSMIndexStart = 4;

	for (unsigned int i = 0; i < NUM_CASCADES * MAX_LIGHTS; ++i)
	{
		currentShader->uploadUniform("shadowMap[" + std::to_string(i) + "]",
			static_cast<int>(CSMIndexStart + i));

		glActiveTexture(GL_TEXTURE0 + CSMIndexStart + i);
		glBindTexture(GL_TEXTURE_2D, _csmTextures[i]);
	}

	currentShader->uploadUniform("numPointLights", static_cast<int>(_pointLights.size()));
	currentShader->uploadUniform("numDirectionalLights", static_cast<int>(_directionalLights.size()));

	currentShader->uploadUniform("viewPos", _cameraPosition);
	currentShader->uploadUniform("celThresholds", _celThresholds);
	currentShader->uploadUniform("color", _color);

	currentShader->uploadUniform("model", modelTransform);
	currentShader->uploadUniform("mvp", mvp);
	currentShader->uploadUniform("texUnit", 0);

	for (unsigned int i = 0; i < NUM_CASCADES; ++i)
	{
		glm::vec4 cascadeEndVec{ 0.f, 0.f, _cascadeEnds[i + 1], 1.f };

		glm::vec4 vClip = _projection * cascadeEndVec;

		currentShader->uploadUniform("CSMEndClipSpace[" + std::to_string(i) + "]", vClip.z);
	}

	std::string texture = modelNode->getTexture();

	if (!texture.empty())
	{
		Texture2D *tex = _assetManager->fetch<Texture2D>(texture);
		tex->bind(0);
		currentShader->uploadUniform("useTexture", 1);
	}
	else
	{
		currentShader->uploadUniform("useTexture", 0);
	}

	if (model->getWireframe())
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	_shader.uploadUniform("terrain", false);

	for (unsigned int i = 0; i < model->getMeshes().size(); ++i)
	{
		unsigned int materialIndex = model->getMeshes().at(i)->getMaterialIndex();

		Material material = model->getMaterials().at(materialIndex);

		currentShader->uploadUniform("material.ambient", material.ambientColor);
		currentShader->uploadUniform("material.diffuse", material.diffuseColor);
		currentShader->uploadUniform("material.specular", material.specularColor);
		currentShader->uploadUniform("material.exponent", material.exponent);

		model->getMeshes().at(i)->render();
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	GLSLShader::use(0);

	if (!modelNode->getOutline())
	{
		return;
	}

	// Render outline box.

	AABB extents = model->getExtents();

	glm::vec3 size = extents.getSize();
	glm::vec3 center = extents.getCenter();
	glm::mat4 transform = glm::mat4{ 1.f };
	transform = glm::translate(transform, center);
	transform = glm::scale(transform, size);

	_outlinesBoxShader.use();

	_outlinesBoxShader.uploadUniform("mvp", _projection * _cameraTransform * modelTransform * transform);
	_outlinesBoxShader.uploadUniform("color", glm::vec3{ 0.f, 0.f, 1.f });

	glBindVertexArray(_boxVAO);

	glDrawArrays(GL_LINES, 0, 24);
	glBindVertexArray(0);

	GLSLShader::use(0);
}

void Renderer::renderStaticModelPicking(
	const StaticModelSceneNode *modelNode)
{
	std::string modelTag = modelNode->getModel();

	const Model *model = _assetManager->fetch<Model>(modelTag);

	Frustum frustum{ _projection * _cameraTransform };

	std::vector<glm::vec3> points = model->getExtents().getPoints();

	glm::mat4 modelTransform = modelNode->getTransformationMatrix() * model->getCorrectionTransform();

	for (auto& it : points)
	{
		it = modelTransform * glm::vec4{ it, 1.f };
	}

	if (frustum.boxIntersect(points) < 0)
	{
		return;
	}

	glm::mat4 mvp = _projection * _cameraTransform * modelNode->getTransformationMatrix() * model->getCorrectionTransform();

	_pickingShader.use();

	_pickingShader.uploadUniform("mvp", mvp);
	_pickingShader.uploadUniform("objectIndex", static_cast<int>(modelNode->getID()));

	for (unsigned int i = 0; i < model->getMeshes().size(); ++i)
	{
		_pickingShader.uploadUniform("drawIndex", static_cast<int>(i));
		model->getMeshes().at(i)->render();
	}

	GLSLShader::use(0);
}

void Renderer::renderStaticModelCSM(
	const StaticModelSceneNode *modelNode)
{
	glViewport(0, 0, _shadowSize, _shadowSize);

	std::string modelTag = modelNode->getModel();

	const Model *model = _assetManager->fetch<Model>(modelTag);

	Frustum frustum{ _projection * _cameraTransform };

	std::vector<glm::vec3> points = model->getExtents().getPoints();

	glm::mat4 modelTransform = modelNode->getTransformationMatrix() * model->getCorrectionTransform();

	for (auto& it : points)
	{
		it = modelTransform * glm::vec4{ it, 1.f };
	}

	if (frustum.boxIntersect(points) < 0)
	{
		return;
	}

	glm::mat4 mvp =
		_orthoProjections[_currentCascade] *
		_lightViewMatrices[_currentCascade] *
		modelNode->getTransformationMatrix() * model->getCorrectionTransform();

	_csmShader.use();

	_csmShader.uploadUniform("mvp", mvp);

	for (unsigned int i = 0; i < model->getMeshes().size(); ++i)
	{
		model->getMeshes().at(i)->render();
	}

	GLSLShader::use(0);
}

void Renderer::renderStaticModelGodrayOcclusion(
	const StaticModelSceneNode *modelNode)
{
	std::string modelTag = modelNode->getModel();

	const Model *model = _assetManager->fetch<Model>(modelTag);

	const glm::mat4 mvp = _projection * _cameraTransform * modelNode->getTransformationMatrix() * model->getCorrectionTransform();

	_godrayOcclusionShader.use();

	_godrayOcclusionShader.uploadUniform("mvp", mvp);
	_godrayOcclusionShader.uploadUniform("color", glm::vec3{ 0.f, 0.f, 0.f });

	for (unsigned int i = 0; i < model->getMeshes().size(); ++i)
	{
		model->getMeshes().at(i)->render();
	}

	GLSLShader::use(0);
}

void Renderer::renderDirectionalLightGodrayOcclusion(
	const DirectionalLightSceneNode *lightNode)
{
	glm::mat4 modelMatrix = glm::translate(glm::mat4{ 1.f }, -lightNode->getDirectionalLight().getDirection());
	modelMatrix = glm::translate(modelMatrix, _cameraPosition);
	modelMatrix = glm::scale(modelMatrix, glm::vec3{ 0.01f, 0.01f, 0.01f });

	glm::mat4 skyboxView = _cameraTransform;

	skyboxView = glm::inverse(skyboxView);

	skyboxView[0][3] = 0.f;
	skyboxView[1][3] = 0.f;
	skyboxView[2][3] = 0.f;

	skyboxView = glm::inverse(skyboxView);

	const glm::mat4 mvp = _projection * skyboxView * modelMatrix;

	_godrayOcclusionShader.use();

	_godrayOcclusionShader.uploadUniform("mvp", mvp);
	_godrayOcclusionShader.uploadUniform("color", lightNode->getDirectionalLight().getDiffuse());

	glDisable(GL_DEPTH_TEST);

	for (unsigned int i = 0; i < _sphereModel.getMeshes().size(); ++i)
	{
		_sphereModel.getMeshes().at(i)->render();
	}

	glEnable(GL_DEPTH_TEST);

	GLSLShader::use(0);
}

void Renderer::renderTerrain(
	const TerrainSceneNode *terrainNode)
{
	glm::mat4 modelMatrix = terrainNode->getTransformationMatrix();

	Terrain *terrain = _assetManager->fetch<Terrain>(terrainNode->getTerrain());

	const glm::mat4 mvp = _projection * _cameraTransform * modelMatrix;

	_shader.use();

	for (auto it : terrain->getChunks())
	{
		glm::mat4 chunkOffset = glm::translate(glm::mat4{ 1.f }, glm::vec3{ it->getOffsetX(), 0.f, it->getOffsetZ() });

		for (unsigned int i = 0; i < _pointLights.size(); ++i)
		{
			_shader.uploadUniform("pointLight[" + std::to_string(i) + "].constant", _pointLights.at(i).first.getConstant());
			_shader.uploadUniform("pointLight[" + std::to_string(i) + "].linear", _pointLights.at(i).first.getLinear());
			_shader.uploadUniform("pointLight[" + std::to_string(i) + "].quadratic", _pointLights.at(i).first.getQuadratic());
			_shader.uploadUniform("pointLight[" + std::to_string(i) + "].ambient", _pointLights.at(i).first.getAmbient());
			_shader.uploadUniform("pointLight[" + std::to_string(i) + "].diffuse", _pointLights.at(i).first.getDiffuse());
			_shader.uploadUniform("pointLight[" + std::to_string(i) + "].specular", _pointLights.at(i).first.getSpecular());
			_shader.uploadUniform("pointLight[" + std::to_string(i) + "].position", _pointLights.at(i).second);
		}

		for (unsigned int i = 0; i < _directionalLights.size(); ++i)
		{
			_shader.uploadUniform("directionalLight[" + std::to_string(i) + "].ambient", _directionalLights.at(i).first.getAmbient());
			_shader.uploadUniform("directionalLight[" + std::to_string(i) + "].diffuse", _directionalLights.at(i).first.getDiffuse());
			_shader.uploadUniform("directionalLight[" + std::to_string(i) + "].specular", _directionalLights.at(i).first.getSpecular());
			_shader.uploadUniform("directionalLight[" + std::to_string(i) + "].direction", _directionalLights.at(i).first.getDirection());

			for (unsigned int j = 0; j < NUM_CASCADES; ++j)
			{
				glm::mat4 lightMVP = _orthoProjections[i * NUM_CASCADES + j] * _lightViewMatrices[i * NUM_CASCADES + j] * modelMatrix * chunkOffset;

				_shader.uploadUniform("lightMVP[" + std::to_string(i * NUM_CASCADES + j) + "]", lightMVP);
			}
		}

		const unsigned int CSMIndexStart = 4;

		for (unsigned int i = 0; i < NUM_CASCADES * MAX_LIGHTS; ++i)
		{
			_shader.uploadUniform("shadowMap[" + std::to_string(i) + "]",
				static_cast<int>(CSMIndexStart + i));

			glActiveTexture(GL_TEXTURE0 + CSMIndexStart + i);
			glBindTexture(GL_TEXTURE_2D, _csmTextures[i]);
		}

		_shader.uploadUniform("numPointLights", static_cast<int>(_pointLights.size()));
		_shader.uploadUniform("numDirectionalLights", static_cast<int>(_directionalLights.size()));
		_shader.uploadUniform("viewPos", _cameraPosition);
		_shader.uploadUniform("celThresholds", _celThresholds);
		_shader.uploadUniform("color", _color);

		_shader.uploadUniform("mvp", mvp * chunkOffset);
		_shader.uploadUniform("model", modelMatrix * chunkOffset);

		for (unsigned int i = 0; i < NUM_CASCADES; ++i)
		{
			glm::vec4 cascadeEndVec{ 0.f, 0.f, _cascadeEnds[i + 1], 1.f };

			glm::vec4 vClip = _projection * cascadeEndVec;

			_shader.uploadUniform("CSMEndClipSpace[" + std::to_string(i) + "]", vClip.z);
		}

		_shader.uploadUniform("useTexture", 0);

		Material material;

		material.ambientColor = glm::vec3{ 0.2f, 0.2f, 0.2f };
		material.diffuseColor = glm::vec3{ 0.3f, 0.8f, 0.3f };
		material.specularColor = glm::vec3{ 0.f, 0.f, 0.f };
		material.exponent = 4;

		_shader.uploadUniform("material.ambient", material.ambientColor);
		_shader.uploadUniform("material.diffuse", material.diffuseColor);
		_shader.uploadUniform("material.specular", material.specularColor);
		_shader.uploadUniform("material.exponent", material.exponent);

		_shader.uploadUniform("terrain", true);

		Frustum frustum{ _projection * _cameraTransform };

		it->render(frustum);
	}

	GLSLShader::use(0);
}

void Renderer::renderTerrainCSM(
	const TerrainSceneNode *terrainNode)
{
	glViewport(0, 0, _shadowSize, _shadowSize);

	Terrain *terrain = _assetManager->fetch<Terrain>(terrainNode->getTerrain());

	_csmShader.use();

	Frustum frustum{ _projection * _cameraTransform };

	for (auto it : terrain->getChunks())
	{
		glm::mat4 chunkOffset = glm::translate(glm::mat4{ 1.f }, glm::vec3{ it->getOffsetX(), 0.f, it->getOffsetZ() });

		glm::mat4 mvp =
			_orthoProjections[_currentCascade] *
			_lightViewMatrices[_currentCascade] *
			terrainNode->getTransformationMatrix() *
			chunkOffset;

		_csmShader.uploadUniform("mvp", mvp);

		it->render(frustum);
	}
}

void Renderer::renderTerrainPicking(
	const TerrainSceneNode *modelNode)
{
	std::string terrainTag = modelNode->getTerrain();

	const Terrain *terrain = _assetManager->fetch<Terrain>(terrainTag);

	glm::mat4 mvp = _projection * _cameraTransform * modelNode->getTransformationMatrix();

	_pickingShader.use();

	_pickingShader.uploadUniform("mvp", mvp);
	_pickingShader.uploadUniform("objectIndex", static_cast<int>(modelNode->getID()));

	unsigned int index = 0;

	Frustum frustum{ _projection * _cameraTransform };

	for (auto it : terrain->getChunks())
	{
		_pickingShader.uploadUniform("drawIndex", static_cast<int>(index++));

		it->render(frustum);
	}

	GLSLShader::use(0);
}
