#include "Player.h"

#include "Game.h"
#include "Terrain.h"

Player::Player(Game *game)
	:_game{game}
{
	_sceneNode = new StaticModelSceneNode{ "warrior", "player" };
	_sceneNode->setPosition(glm::vec3{ 0.f, 0.f, 0.f });
	_sceneNode->setRotation(glm::vec3{ 0.f, 0.f, 0.f });
	_sceneNode->setScale(glm::vec3{ 0.1f, 0.1f, 0.1f });
	_sceneNode->setTexture("char");

	game->addToRoot(_sceneNode);
}

void Player::handlePlayerMovement(const InputManager& inputManager, float dt, Terrain *terrain)
{
	static float oldX = 0.f;
	static float oldY = 0.f;

	glm::vec3 deltaPos = glm::vec3{ 0.f, 0.f, 0.f };

	static float lastHeight = 0;

	//=========================================================================
	// Handle camera rotation
	//=========================================================================

	if (inputManager.leftClick || inputManager.rightClick)
	{
		float deltaX = inputManager.mouseX - oldX;
		float deltaY = inputManager.mouseY - oldY;

		_phi += 0.01f * deltaX;
		_theta += 0.005f * deltaY;

		if (_theta < -glm::radians(89.f))
		{
			_theta = -glm::radians(89.f);
		}

		if (_theta > glm::radians(89.f))
		{
			_theta = glm::radians(89.f);
		}
	}

	if (inputManager.rightClick)
	{
		_lockFacing = true;
	}
	else
	{
		_lockFacing = false;
	}

	//=========================================================================
	// Handle player position
	//=========================================================================

	_walking = false;

	if(inputManager.wKey || (inputManager.rightClick && inputManager.leftClick))
	{
		// Move forward
		deltaPos.x -= glm::cos(_facing);
		deltaPos.z += glm::sin(_facing);
		_walking = true;
	}

	if (inputManager.sKey)
	{
		// Move backward
		deltaPos.x += glm::cos(_facing);
		deltaPos.z -= glm::sin(_facing);
		_walking = true;
	}

	if (inputManager.aKey)
	{
		if(!_lockFacing)
		{
			// Rotate left
			_facing += _rotationSpeed;
		}
		else
		{
			// Strafe left
			deltaPos.x += glm::sin(_facing);
			deltaPos.z += glm::cos(_facing);
			_walking = true;
		}
	}
	if (inputManager.dKey)
	{
		if (!_lockFacing)
		{
			// Rotate right
			_facing -= _rotationSpeed;
		}
		else
		{
			// Strafe right
			deltaPos.x -= glm::sin(_facing);
			deltaPos.z -= glm::cos(_facing);
			_walking = true;
		}
	}
	// TODO: Add jump

	oldX = inputManager.mouseX;
	oldY = inputManager.mouseY;

	float height = lastHeight;

	if(glm::length(deltaPos) > 0.1f)
	{
		deltaPos = _speed * glm::normalize(deltaPos);

		float heightAtDest = terrain->getHeight(_position.x + deltaPos.x, _position.z + deltaPos.z);

		float slope = (heightAtDest - _position.y) / _speed;

		if(slope < 1.f)
		{
			_position.x += deltaPos.x;
			_position.z += deltaPos.z;

			height = heightAtDest;
		}
	}
	else
	{
		height = terrain->getHeight(_position.x, _position.z);
	}

	if(_position.y < height)
	{
		_yVel = 0;
		_position.y = height;
		_touchingGround = true;
	}
	else if(!_touchingGround || lastHeight > height)
	{
		_yVel -= 9.82f * dt / 20.f;
		
	}

	_position.y += _yVel;

	lastHeight = height;

	_sceneNode->setPosition(_position);

	if(_lockFacing)
	{
		_facing = -_phi;
	}

	_sceneNode->setRotation(glm::vec3{ 0.f, glm::degrees(_facing), 0.f });

	if(_walking)
	{
		_sceneNode->setCurrentAnimation("anim0");
	}
	else
	{
		_sceneNode->setCurrentAnimation("idle");
	}

	glm::vec3 cameraCenter = _position + glm::vec3(0.f, 1.f, 0.f);

	glm::vec3 eye;

	eye.x = cameraCenter.x + _radius * glm::cos(_phi) * glm::cos(_theta);
	eye.y = cameraCenter.y + _radius * glm::sin(_theta);
	eye.z = cameraCenter.z + _radius * glm::sin(_phi) * glm::cos(_theta);

	float heightAtCamera = terrain->getHeight(eye.x, eye.z);

	if (eye.y <= heightAtCamera + 1.f)
	{
		eye.y = heightAtCamera + 1.f;
	}

	glm::mat4 view = glm::lookAt(eye, cameraCenter, glm::vec3{ 0.f, 1.f, 0.f });

	_game->getApplication()->getRenderer()->setCameraTransform(view);
	_game->getApplication()->getRenderer()->setCameraPosition(eye);
	_game->getApplication()->getRenderer()->setCameraDirection(glm::normalize(eye - cameraCenter));
}