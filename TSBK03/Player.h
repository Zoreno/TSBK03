/**
 * @file	Player.h
 * @Author	Joakim Bertils
 * @date	2018-11-07
 * @brief	TODO: Insert documentation
 */

#pragma once

#include "SceneNode.h"
#include "InputManager.h"
#include "StaticModelSceneNode.h"

class Terrain;
class Game;

class Player
{
public:
	Player(
		Game *game);

	void handlePlayerMovement(
		InputManager &inputManager,
		float dt,
		Terrain *terrain);

	glm::vec3 getPosition() const;
	void setPosition(const glm::vec3 &position);
	float getFacing() const;
	void setFacing(float facing);
	float getSpeed() const;
	void setSpeed(float speed);
	bool getTouchingGround() const;
	bool getWalking() const;

	const std::string& getName() const;

	int getLevel() const;
	int getExperience() const;

	int getHealth() const;
	int getMaxHealth() const;

	int getMana() const;
	int getMaxMana() const;

	void addExperience(int value);

	void takeDamage(int value);
private:
	std::string _name{"Kalle Storfiskare"};

	int _level{1};
	int _experience{0};

	int _health{100};
	int _maxHealth{100};

	int _mana{100};
	int _maxMana{100};

	// Player position and orientation
	glm::vec3 _position{220.f, 0.f, 220.f};
	float _facing{0.f};

	float _yVel{0.f};
	float _speed{0.03f};
	float _rotationSpeed{0.05f};
	bool _touchingGround{true};
	bool _walking{false};

	// Camera position
	float _phi{0.f};
	float _theta{0.f};
	float _radius{5.f};
	bool _lockFacing{false};
	Game *_game;

	StaticModelSceneNode *_sceneNode;
};
