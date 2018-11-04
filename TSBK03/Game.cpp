#include "Game.h"

Game::Game(
	Application *application)
	: Frame(application)
{
	_scene = new Scene{ SceneNodeType::ROOT, "ROOT" };

	TerrainSceneNode *terrainNode = new TerrainSceneNode("terrain", "terrain");
	terrainNode->setPosition(glm::vec3{ 0.f, 0.f, 0.f });
	terrainNode->setRotation(glm::vec3{ 0.f, 0.f, 0.f });
	terrainNode->setScale(glm::vec3{ 1.f, 1.f, 1.f });

	_scene->addChild(terrainNode);

	DirectionalLightSceneNode *directionalLight1 = new DirectionalLightSceneNode{
		DirectionalLight{
		glm::vec3{ 3.f, -3.f, 3.f },
		glm::vec3{ 0.2f, 0.2f, 0.2f },
		glm::vec3{ 0.98f, 0.98f, 0.90f },
		glm::vec3{ 0.8f, 0.8f, 0.8f } },
		"DirectionalLight1" };

	_scene->addChild(directionalLight1);
}

void Game::update(
	float dt)
{
	
}

void Game::render(
	Renderer *renderer)
{
	
}
