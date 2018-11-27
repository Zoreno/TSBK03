#include "Player.h"

#include "Game.h"
#include "Terrain.h"
#include <numeric>

Player::Player(Game *game)
	:_game{ game },
	_inventory{ 64, game->getItemDatabase() },
	_equipmentManager{ game }
{
	_sceneNode = new StaticModelSceneNode{ "warrior", "player" };
	_sceneNode->setPosition(glm::vec3{ 0.f, 0.f, 0.f });
	_sceneNode->setRotation(glm::vec3{ 0.f, 0.f, 0.f });
	_sceneNode->setScale(glm::vec3{ 0.1f, 0.1f, 0.1f });
	_sceneNode->setTexture("char");

	game->addToRoot(_sceneNode);

	// Intialize the base stats for the player.
	_baseStats.agility = 10;
	_baseStats.strength = 12;
	_baseStats.stamina = 17;
	_baseStats.intellect = 9;
	_baseStats.spirit = 14;

	// Make sure that all the stats are updated
	recalculateStats();
}

void Player::handlePlayerMovement(InputManager& inputManager, float dt, Terrain *terrain)
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

		if (glm::abs(deltaX) > 4.f || glm::abs(deltaY) > 4.f)
		{
			inputManager.dragging = true;
		}

		if (_theta < -glm::radians(89.f))
		{
			_theta = -glm::radians(89.f);
		}

		if (_theta > glm::radians(89.f))
		{
			_theta = glm::radians(89.f);
		}

		glfwSetInputMode(_game->getApplication()->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else
	{
		glfwSetInputMode(_game->getApplication()->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		inputManager.dragging = false;
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

	if (inputManager.keys[KEY_W] || (inputManager.rightClick && inputManager.leftClick))
	{
		// Move forward
		deltaPos.x -= glm::cos(_facing);
		deltaPos.z += glm::sin(_facing);
		_walking = true;
	}

	if (inputManager.keys[KEY_S])
	{
		// Move backward
		deltaPos.x += glm::cos(_facing);
		deltaPos.z -= glm::sin(_facing);
		_walking = true;
	}

	if (inputManager.keys[KEY_A])
	{
		if (!_lockFacing)
		{
			// Rotate left
			_facing += dt * _rotationSpeed;
		}
		else
		{
			// Strafe left
			deltaPos.x += glm::sin(_facing);
			deltaPos.z += glm::cos(_facing);
			_walking = true;
		}
	}

	if (inputManager.keys[KEY_D])
	{
		if (!_lockFacing)
		{
			// Rotate right
			_facing -= dt * _rotationSpeed;
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

	if (glm::length(deltaPos) > 0.1f)
	{
		deltaPos = dt * _speed * glm::normalize(deltaPos);

		float heightAtDest = terrain->getHeight(_position.x + deltaPos.x, _position.z + deltaPos.z);

		float slope = (heightAtDest - _position.y) / _speed;

		if (slope < 1.f)
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

	if (_position.y < height)
	{
		_yVel = 0;
		_position.y = height;
		_touchingGround = true;
	}
	else if (!_touchingGround || lastHeight > height)
	{
		_yVel -= 9.82f * dt / 20.f;

	}

	_position.y += _yVel;

	lastHeight = height;

	_sceneNode->setPosition(_position);

	if (_lockFacing)
	{
		_facing = -_phi;
	}

	_sceneNode->setRotation(glm::vec3{ 0.f, glm::degrees(_facing), 0.f });

	if (_walking)
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

	if(_cooldown > 0.f)
	{
		_cooldown -= dt;
	}

	// TODO: Mana reg
	if(_health < _maxHealth)
	{
		_healthRegenCounter += dt;

		if(_healthRegenCounter >= 5.f)
		{
			_healthRegenCounter = 0;
			_health += _healthPer5;

			_health = glm::min(_health, _maxHealth);
		}
	}
	else
	{
		_healthRegenCounter = 0.f;
	}
}

glm::vec3 Player::getPosition() const
{
	return _position;
}

void Player::setPosition(
	const glm::vec3 &position)
{
	_position = position;
}

float Player::getFacing() const
{
	return _facing;
}

void Player::setFacing(
	float facing)
{
	_facing = facing;
}

float Player::getSpeed() const
{
	return _speed;
}

void Player::setSpeed(
	float speed)
{
	_speed = speed;
}

bool Player::getTouchingGround() const
{
	return _touchingGround;
}

bool Player::getWalking() const
{
	return _walking;
}

const std::string & Player::getName() const
{
	return _name;
}

int Player::getLevel() const
{
	return _level;
}

int Player::getExperience() const
{
	return _experience;
}

int Player::getHealth() const
{
	return _health;
}

int Player::getMaxHealth() const
{
	return _maxHealth;
}

int Player::getMissingHealth() const
{
	return _maxHealth - _health;
}

int Player::getMana() const
{
	return _mana;
}

int Player::getMaxMana() const
{
	return _maxMana;
}

void Player::addExperience(
	int value)
{
	_experience += value;

	while(checkForLevelup())
	{
		_maxHealth += 20;
		_maxMana += 10;

		_baseStats.agility += 10;

		recalculateStats();
	}
}

int Player::getExperienceToLevel() const
{
	return getExperienceToLevelup(_level);
}

void Player::takeDamage(int value)
{
	_health -= value;
}

Inventory * Player::getInventory()
{
	return &_inventory;
}

EquipmentManager * Player::getEquipmentManager()
{
	return &_equipmentManager;
}

void Player::attack(
	Enemy *enemy)
{
	if (enemy->distanceToPlayer() < 3)
	{
		if (_cooldown <= 0.f)
		{
			int damage = _game->getRandomGenerator()->randInt32Range(2*_stats.agility, 4*_stats.agility);

			std::cout << damage << std::endl;

			enemy->takeDamage(damage);
			_cooldown += 1.f;
		}
		else
		{
			std::cout << "Spell is on cooldown" << std::endl;
		}
	}
	else
	{
		std::cout << "Enemy out of range" << std::endl;
	}
}

void Player::heal(
	int value)
{
	_health = glm::min(_health + value, _maxHealth);
}

void Player::recalculateStats()
{
	Stats itemStats = getEquipmentManager()->getStats();

	_stats = _baseStats + itemStats;

	_maxHealth = _stats.stamina * 10;

	if(_health > _maxHealth)
	{
		_health = _maxHealth;
	}
}

const Stats & Player::getBaseStats() const
{
	return _baseStats;
}

const Stats & Player::getStats() const
{
	return _stats;
}

int Player::getExperienceToLevelup(
	int level) const
{
	return 50 * glm::ceil(glm::pow(_level, 2.6f));
}

bool Player::checkForLevelup()
{
	int expToLevel = getExperienceToLevelup(_level);

	if(_experience >= expToLevel)
	{
		_experience -= expToLevel;
		++_level;

		return true;
	}

	return false;
}
