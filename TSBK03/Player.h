#pragma once

#include "SceneNode.h"
#include "InputManager.h"
#include "StaticModelSceneNode.h"

class Terrain;
class Game;

class Player
{
public:
	Player(Game *game);

	void handlePlayerMovement(const InputManager& inputManager, float dt, Terrain *terrain);
private:

	// Player position
	glm::vec3 _position{ 220.f, 0.f, 220.f };
	float _facing{ 0.f };
	float _yVel{ 0.f };

	float _speed{ 0.03f };
	float _rotationSpeed{ 0.05f };
	bool _touchingGround{true};
	bool _walking{false};

	// Camera position

	float _phi{0.f};
	float _theta{0.f};

	float _radius{10.f};

	bool _lockFacing{false};

	Game *_game;


	StaticModelSceneNode *_sceneNode;
};
