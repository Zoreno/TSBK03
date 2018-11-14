#pragma once

#include "SceneNode.h"
#include "StaticModelSceneNode.h"
#include "Terrain.h"

class Game;

class Enemy
{
public:
	Enemy(int id, Game *game, const glm::vec3& position, Terrain *terrain);

	virtual ~Enemy();
	void renderUI(bool isTarget);

	virtual void update(float dt, Terrain *terrain);

	void setShowNamePlate(bool val);

	unsigned int getSceneNodeID() const;

	int getHealth() const;
	int getMaxHealth() const;

	int getMana() const;
	int getMaxMana() const;

	const std::string& getName() const;

	int getLevel() const;

	void takeDamage(int damage);

	virtual void onDeath();
	virtual void onTargeted();
	virtual void onUntargeted();

	float distanceToPlayer() const;
	float distanceToStart() const;

	// TODO: Move terrain reference to game (zone)
	void move(float dx, float dz, Terrain *terrain);
	void teleport(float x, float z, Terrain *terrain);
protected:

	void updateState(float dt, Terrain *terrain);
	void executeState(float dt, Terrain *terrain);
	void attackPlayer(float dt);
	enum class AiState { ATTACK, MOVETOPLAYER, IDLE, RESET };
	AiState _state{ AiState::IDLE };

	float _attackTimer{0};
	float _resetTimer{0};

	std::string _name{ "NONAME" };

	glm::vec3 _startPosition;

	int _level{ 1 };

	int _health{ 100 };
	int _maxHealth{ 100 };

	int _mana{ 100 };
	int _maxMana{ 100 };

	int _id;

	glm::vec3 _position;
	float _facing;

	StaticModelSceneNode *_sceneNode;

	bool _showNamePlate{ false };

	Game *_game;
};
