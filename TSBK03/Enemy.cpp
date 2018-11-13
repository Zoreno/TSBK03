#include "Enemy.h"

#include "Game.h"
#include "imgui.h"

Enemy::Enemy(
	int id,
	Game *game,
	const glm::vec3& position,
	Terrain *terrain)
	: _id{ id },
	_game{ game }
{
	glm::vec3 posOnSurface = position;
	posOnSurface.y = terrain->getHeight(position.x, position.z);

	_position = posOnSurface;
	_startPosition = posOnSurface;
	_facing = 0.f;

	_sceneNode = new StaticModelSceneNode{ "warrior", "enemy" };
	_sceneNode->setPosition(_position);
	_sceneNode->setRotation(glm::vec3{ 0.f, glm::degrees(_facing), 0.f });
	_sceneNode->setScale(glm::vec3{ 0.1f, 0.1f, 0.1f });
	_sceneNode->setTexture("char");

	game->addToRoot(_sceneNode);
}

Enemy::~Enemy()
{
	_game->removeFromRoot(_sceneNode);
}

void Enemy::renderUI(bool isTarget)
{
	if (_showNamePlate || isTarget)
	{
		glm::vec2 ssPos = _game->getApplication()->getRenderer()->getScreenSpacePosition(_position + glm::vec3(0.f, 1.f, 0.f), false);

		std::string str{ "nameplate##" };
		str += std::to_string(_id);

		ImGui::PushID(_id);
		ImGui::SetNextWindowPos(ImVec2(ssPos.x, ssPos.y), 0, ImVec2(0.5f, 1.0f));
		if (isTarget)
		{
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.75f, 0.f, 0.f, 1.f));
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.f);
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 1.f));
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.3f);
		}
		ImGui::Begin(str.c_str(), &_showNamePlate, ImVec2(150.f, 0), 0.9f,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);

		ImGui::SetWindowFontScale(0.8f);
		ImGui::Text("[%i] %s", _level, _name.c_str());
		ImGui::SetWindowFontScale(1.f);

		{
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.75f, 0.f, 0.f, 1.f));

			float fraction = (float)_health / (float)_maxHealth;

			//std::string str = std::to_string(_health) + std::string{ "/" } +std::to_string(_maxHealth);
			std::string str{};

			ImGui::ProgressBar(fraction, ImVec2(-1, 10), str.c_str());
			ImGui::PopStyleColor();
		}

		{
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.f, 0.f, 0.75f, 1.f));

			float fraction = (float)_mana / (float)_maxMana;

			//std::string str = std::to_string(_mana) + std::string{ "/" } +std::to_string(_maxMana);
			std::string str{};

			ImGui::ProgressBar(fraction, ImVec2(-1, 3), str.c_str());
			ImGui::PopStyleColor();
		}

		ImGui::End();

		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		ImGui::PopID();
	}
}

void Enemy::update(
	float dt,
	Terrain *terrain)
{
	_sceneNode->setCurrentAnimation("idle");

	// Update the Ai
	updateState(dt, terrain);
	executeState(dt, terrain);

	// Update the scene node
	_sceneNode->setPosition(_position);
	_sceneNode->setRotation(glm::vec3{ 0.f, glm::degrees(_facing), 0.f });

	// Determine whether the nameplate should be shown.
	if (distanceToPlayer() < 10.f)
	{
		setShowNamePlate(true);
	}
	else
	{
		setShowNamePlate(false);
	}

}

void Enemy::setShowNamePlate(
	bool val)
{
	_showNamePlate = val;
}

unsigned int Enemy::getSceneNodeID() const
{
	return _sceneNode->getID();
}

int Enemy::getHealth() const
{
	return _health;
}

int Enemy::getMaxHealth() const
{
	return _maxHealth;
}

int Enemy::getMana() const
{
	return _mana;
}

int Enemy::getMaxMana() const
{
	return _maxMana;
}

const std::string & Enemy::getName() const
{
	return _name;
}

int Enemy::getLevel() const
{
	return _level;
}

void Enemy::takeDamage(
	int damage)
{
	_health -= damage;
}

void Enemy::onDeath()
{
	_game->getPlayer()->addExperience(25);

	_game->getPlayer()->getInventory()->addItem(ItemInstance{ 0, 1 });
}

void Enemy::onTargeted()
{
	_sceneNode->setTintColor(glm::vec3(0.2f, 0.f, 0.f));
}

void Enemy::onUntargeted()
{
	_sceneNode->setTintColor(glm::vec3(0.f, 0.f, 0.f));
}

float Enemy::distanceToPlayer() const
{
	return glm::length(_position - _game->getPlayer()->getPosition());
}

float Enemy::distanceToStart() const
{
	return glm::length(_position - _startPosition);
}

void Enemy::move(
	float dx,
	float dz,
	Terrain *terrain)
{
	_position.x += dx;
	_position.z += dz;

	_position.y = terrain->getHeight(_position.x, _position.z);

	_facing = glm::atan(dz, -dx);

	_sceneNode->setCurrentAnimation("anim0");
}

void Enemy::teleport(
	float x,
	float z,
	Terrain *terrain)
{
	_position.x = x;
	_position.z = z;

	_position.y = terrain->getHeight(_position.x, _position.z);
}

void Enemy::updateState(float dt, Terrain *terrain)
{
	float playerDistance = distanceToPlayer();
	float startDistance = distanceToStart();

	switch (_state)
	{
	case AiState::ATTACK:
		if (startDistance > 10 || playerDistance > 10)
		{
			_state = AiState::RESET;
		}
		else if (playerDistance > 1)
		{
			_state = AiState::MOVETOPLAYER;
		}
		break;
	case AiState::MOVETOPLAYER:
		if (startDistance > 20 || playerDistance > 20)
		{
			_state = AiState::RESET;
		}
		else if (playerDistance <= 1)
		{
			_state = AiState::ATTACK;
		}
		break;
	case AiState::IDLE:
		if (playerDistance <= 10)
		{
			_state = AiState::MOVETOPLAYER;
		}
		break;
	case AiState::RESET:
		if (startDistance < 2)
		{
			_state = AiState::IDLE;
		}
		break;
	default:;
	}
}

void Enemy::executeState(float dt, Terrain *terrain)
{
	glm::vec3 playerPosFlat = _game->getPlayer()->getPosition();
	playerPosFlat.y = 0;

	glm::vec3 startPosFlat = _startPosition;
	startPosFlat.y = 0;

	glm::vec3 posFlat = _position;
	posFlat.y = 0;

	switch (_state)
	{
	case AiState::ATTACK:
		attackPlayer();
		break;
	case AiState::MOVETOPLAYER:
	{
		glm::vec3 toPlayerVector = glm::normalize(playerPosFlat - posFlat);

		move(dt * toPlayerVector.x, dt * toPlayerVector.z, terrain);
	}
	break;
	case AiState::IDLE:
		_resetTimer = 0;
		break;
	case AiState::RESET:
		_health = _maxHealth;
		if (_resetTimer >= 300)
		{
			teleport(_startPosition.x, _startPosition.z, terrain);
		}
		else
		{
			++_resetTimer;
			glm::vec3 toStartVector = glm::normalize(startPosFlat - posFlat);

			move(dt * 2.f * toStartVector.x, dt * 2.f * toStartVector.z, terrain);
		}
		break;
	default:;
	}
}

void Enemy::attackPlayer()
{
	if(_attackTimer >= 60)
	{
		_game->getPlayer()->takeDamage(10);
		_attackTimer = 0;
		return;
	}
	++_attackTimer;
}

