#include "Game.h"

Game::Game(
	Application *application)
	: Frame(application),
	_scene{ new Scene{ SceneNodeType::ROOT, "ROOT" } },
	_player{ this }
{

	_terrainNode = new TerrainSceneNode("terrain", "terrain");
	_terrainNode->setPosition(glm::vec3{ 0.f, 0.f, 0.f });
	_terrainNode->setRotation(glm::vec3{ 0.f, 0.f, 0.f });
	_terrainNode->setScale(glm::vec3{ 1.f, 1.f, 1.f });

	_scene->addChild(_terrainNode);

	DirectionalLightSceneNode *directionalLight1 = new DirectionalLightSceneNode{
		DirectionalLight{
		glm::vec3{ 3.f, -3.f, 3.f },
		glm::vec3{ 0.2f, 0.2f, 0.2f },
		glm::vec3{ 0.98f, 0.98f, 0.90f },
		glm::vec3{ 0.8f, 0.8f, 0.8f } },
		"DirectionalLight1" };

	_scene->addChild(directionalLight1);

	application->getEventManager()->addSubscriber<MouseMovedEvent>(&_inputManager);
	application->getEventManager()->addSubscriber<MouseButtonEvent>(&_inputManager);
	application->getEventManager()->addSubscriber<KeyEvent>(&_inputManager);
}

void Game::update(
	float dt)
{
	Terrain *terrain = _application->getAssetManager()->fetch<Terrain>(_terrainNode->getTerrain());

	_player.handlePlayerMovement(_inputManager, dt, terrain);
}

void Game::renderUI()
{

}

void Game::render(
	Renderer *renderer)
{
	renderer->render(_scene);
}

void Game::addToRoot(
	SceneNode *sceneNode)
{
	_scene->addChild(sceneNode);
}
