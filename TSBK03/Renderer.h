#pragma once
#include "SceneNode.h"

#include "PointLight.h"
#include "DirectionalLight.h"
#include "RendererPickingInfo.h"

#include "GLSLShader.h"
#include "StaticModelSceneNode.h"
#include "Texture.h"

#include "AssetManager.h"
#include "Skybox.h"
#include "DirectionalLightSceneNode.h"
#include "TerrainSceneNode.h"

#define MAX_LIGHTS 8
#define NUM_CASCADES 3

class Model;

// TODO: It might be possible to cache some of the models and textures

class Renderer
{
public:
	Renderer(int windowWidth, int windowHeight, AssetManager *assetManager, glm::mat4 projection);

	~Renderer();

	void render(Scene *scene);

	void setCameraTransform(const glm::mat4& cameraTransform);

	void setProjection(const glm::mat4& projection);

	void setCameraPosition(const glm::vec3& cameraPos);
	void setCameraDirection(const glm::vec3& newDirection);

	float getLastGPURenderTime() const;

	glm::vec3 _celThresholds{ 0.1f, 0.3f, 0.6f };

	glm::vec4 _color{ 1.f, 1.f, 1.f, 1.f };

	void changeResolution(int newWidth, int newHeight);

	int getWindowWidth() const;
	int getWindowHeight() const;

	bool getHdr() const;
	bool getBloom() const;

	float getExposure() const;
	float getGamma() const;

	void setHdr(bool hdr);
	void setBloom(bool bloom);

	void setExposure(float exposure);
	void setGamma(float gamma);

	RendererPickingInfo getPickingInfo(int x, int y);

	bool getFXAA() const;
	bool getShowEdges() const;

	float getLumaThreshold() const;
	float getMulReduce() const;
	float getMinReduce() const;
	float getMaxSpan() const;

	void setFXAA(bool fxaa);
	void setShowEdges(bool showEdges);

	void setLumaThreshold(float lumaThreshold);
	void setMulReduce(float mulReduce);
	void setMinReduce(float minReduce);
	void setMaxSpan(float maxSpan);

	void setSkybox(const Skybox& skybox);

	const GLuint const *getCSMTextures() const;

	GLuint getGodrayOcclusionTexture() const;

	float getGodrayDensity()const;
	float getGodrayWeight() const;
	float getGodrayDecay() const;
	float getGodrayExposure() const;
	float getGodrayNumSamples() const;

	void setGodrayDensity(float newDensity);
	void setGodrayWeight(float newWeight);
	void setGodrayDecay(float newDecay);
	void setGodrayExposure(float newExposure);
	void setGodrayNumSamples(float newNumSamples);
private:

	void extractLights(
		const SceneNode *node);

	void renderSkybox();

	void doGodrayOcclusionRenderingPass(
		const SceneNode *scene);

	void doColorRenderingPass(
		const SceneNode *scene);

	void doPickingRenderPass(
		const SceneNode *scene);

	void doBloomBlurRenderingPass();

	void doScreenRenderPass();

	void renderStaticModelGodrayOcclusion(
		const StaticModelSceneNode * modelNode);

	void renderDirectionalLightGodrayOcclusion(
		const DirectionalLightSceneNode * lightNode);

	void renderTerrain(
		const TerrainSceneNode * terrainNode);

	void renderTerrainCSM(
		const TerrainSceneNode * terrainNode);

	void renderScene(
		const SceneNode *node,
		int pass);

	void renderStaticModel(
		const StaticModelSceneNode *modelNode);

	void renderStaticModelPicking(
		const StaticModelSceneNode *modelNode);

	void renderStaticModelCSM(
		const StaticModelSceneNode * modelNode);

	GLSLShader _shader{};
	GLSLShader _skinnedShader{};
	GLSLShader _blurShader{};
	GLSLShader _hdrShader{};
	GLSLShader _pickingShader{};
	GLSLShader _skyboxShader{};
	GLSLShader _outlinesBoxShader{};
	GLSLShader _csmShader{};
	GLSLShader _godrayOcclusionShader{};

	glm::mat4 _cameraTransform{};

	glm::mat4 _projection{};

	GLuint _queryID[2];

	float _lastRenderTime{ 0.f };

	glm::vec3 _cameraPosition{};
	glm::vec3 _cameraDirection{};

	std::vector<std::pair<PointLight, glm::vec3>> _pointLights;

	std::vector<std::pair<DirectionalLight, glm::vec3>> _directionalLights;

	int _windowWidth;
	int _windowHeight;

	// Screen Quad
	GLuint _quadVAO{ 0 };
	GLuint _quadVBO{ 0 };

	GLuint _hdrFBO{ 0 };
	GLuint _colorBuffers[2]; // One for regular and one for bright color.
	GLuint _rboDepth{ 0 };

	GLuint _pingpongFBO[2];
	GLuint _pingpongColorbuffers[2];

	bool _hdr{ true };
	bool _bloom{ true };

	float _exposure{ 1.f };
	float _gamma{ 1.f };

	GLuint _pickingFBO{ 0 };
	GLuint _pickingTexture{ 0 };
	GLuint _pickingDepth{ 0 };

	AssetManager *_assetManager{ nullptr };

	// FXAA
	bool _fxaa = true;
	bool _showEdges = false;
	float _lumaThreshold = 1.0 / 8.0;
	float _mulReduce = 1.0 / 8.0;
	float _minReduce = 1.0 / 128.0;
	float _maxSpan = 8.0;

	Skybox _skybox{ "skyboxSphere", "skyboxTexture" };

	GLuint _boxVAO;
	GLuint _boxVBO;

	// We might get away with using only one FBO in this case, but this is
	// easier when we have a 8 GB GTX1080 card :)
	GLuint _csmFBO[MAX_LIGHTS];

	unsigned int _shadowSize{ 1024 };
	GLuint _csmTextures[MAX_LIGHTS * NUM_CASCADES];
	glm::mat4 _orthoProjections[NUM_CASCADES * MAX_LIGHTS];
	glm::mat4 _lightViewMatrices[NUM_CASCADES * MAX_LIGHTS];
	float _cascadeEnds[NUM_CASCADES + 1];
	unsigned int _currentCascade;

	void calculateOrthoProjections(int lightIndex);

	void doCSMShadowPass(
		Scene * scene);

	unsigned int _godrayOcclusionSize{ 1024 };
	GLuint _godrayFBO{ 0 };
	GLuint _godrayTexture{ 0 };
	GLuint _godrayDepth{ 0 };
	Model _sphereModel{ "sphere.obj" };
	float _godrayDensity{ 1.f };
	float _godrayWeight{ 0.1f };
	float _godrayDecay{ 1.f };
	float _godrayExposure{ 1.f };
	float _godrayNumSamples{ 100.f };
};