#include "Game.h"
#include "imgui.h"

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

	Terrain *terrain = _application->getAssetManager()->fetch<Terrain>(_terrainNode->getTerrain());

	_enemies.push_back(new Enemy{ 0, this , glm::vec3{ 200.f, 0.f, 220.f }, terrain });
	_enemies.push_back(new Enemy{ 1, this , glm::vec3{ 240.f, 0.f, 220.f }, terrain });
}

Game::~Game()
{
	// TODO: Cleanup
}

void Game::update(
	float dt)
{
	Terrain *terrain = _application->getAssetManager()->fetch<Terrain>(_terrainNode->getTerrain());

	static bool lastLClick = _inputManager.leftClick;
	static bool lastEKey = _inputManager.eKey;

	// TODO: Target with TAB
	// Detect falling flank on left click
	if (!_inputManager.dragging && lastLClick && !_inputManager.leftClick)
	{
		RendererPickingInfo pickingInfo;

		pickingInfo = _application->getRenderer()->getPickingInfo(_inputManager.mouseX, _inputManager.mouseY);

		bool found = false;

		for (auto it : _enemies)
		{
			if (it->getSceneNodeID() == pickingInfo.objectID)
			{
				_currentTarget = it;

				found = true;

				break;
			}
		}

		if (!found && _currentTarget != nullptr)
		{
			_currentTarget = nullptr;
		}
	}

	_player.handlePlayerMovement(_inputManager, dt, terrain);

	for (auto it : _enemies)
	{
		it->update(dt, terrain);
	}

	if (_inputManager.eKey && !lastEKey)
	{
		if (_currentTarget != nullptr)
		{
			_currentTarget->takeDamage(10);
		}
	}

	std::vector<Enemy *> _enemiesPendingRemove;

	for (auto it = _enemies.begin(); it != _enemies.end();)
	{
		Enemy *enemy = *it;

		if (enemy->getHealth() <= 0)
		{
			// Enemy is dead
			enemy->onDeath();

			if (_currentTarget == enemy)
			{
				_currentTarget = nullptr;
			}

			_enemiesPendingRemove.push_back(enemy);

			it = _enemies.erase(it);
		}
		else
		{
			++it;
		}
	}

	for (auto it : _enemiesPendingRemove)
	{
		delete it;
	}

	_enemiesPendingRemove.clear();

	lastLClick = _inputManager.leftClick;
	lastEKey = _inputManager.eKey;
}

void Game::renderUI()
{
	//=========================================================================
	// Render Player Portrait
	//=========================================================================

	ImGui::SetNextWindowPos(ImVec2(20, 20));
	ImGui::Begin("Portrait", 0, ImVec2(200.f, 0.f), 0.9f,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);

	ImGui::Text("%s", _player.getName().c_str());
	ImGui::Text("Level %i", _player.getLevel());

	{
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.75f, 0.f, 0.f, 1.f));

		int health = _player.getHealth();
		int maxHealth = _player.getMaxHealth();

		float fraction = (float)health / (float)maxHealth;

		std::string str = std::to_string(health) + std::string{ "/" } +std::to_string(maxHealth) + std::string{ " HP" };

		ImGui::ProgressBar(fraction, ImVec2(-1, 20), str.c_str());
		ImGui::PopStyleColor();
	}

	{
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.f, 0.f, 0.75f, 1.f));

		int mana = _player.getMana();
		int maxMana = _player.getMaxMana();

		float fraction = (float)mana / (float)maxMana;

		std::string str = std::to_string(mana) + std::string{ "/" } +std::to_string(maxMana) + std::string{ " MP" };

		ImGui::ProgressBar(fraction, ImVec2(-1, 20), str.c_str());
		ImGui::PopStyleColor();
	}

	{
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(200.f / 256.f, 255.f / 256.f, 121.f / 256.f, 1.f));

		int exp = _player.getExperience();
		int maxExp = 100; // TODO: replace with calculation of max exp for level

		float fraction = (float)exp / (float)maxExp;

		std::string str = std::to_string(exp) + std::string{ "/" } +std::to_string(maxExp) + std::string{ " EXP" };

		ImGui::ProgressBar(fraction, ImVec2(-1, 20), str.c_str());
		ImGui::PopStyleColor();
	}

	ImGui::End();

	//=========================================================================
	// Render Target Portrait
	//=========================================================================
	if (_currentTarget != nullptr)
	{
		ImGui::SetNextWindowPos(ImVec2(240.f, 20));
		ImGui::Begin("TargetPortrait", 0, ImVec2(200.f, 0.f), 0.9f,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);

		ImGui::Text("%s", _currentTarget->getName().c_str());
		ImGui::Text("Level %i", _currentTarget->getLevel());

		{
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.75f, 0.f, 0.f, 1.f));

			int health = _currentTarget->getHealth();
			int maxHealth = _currentTarget->getMaxHealth();

			float fraction = (float)health / (float)maxHealth;

			std::string str = std::to_string(health) + std::string{ "/" } +std::to_string(maxHealth) + std::string{ " HP" };

			ImGui::ProgressBar(fraction, ImVec2(-1, 20), str.c_str());
			ImGui::PopStyleColor();
		}

		{
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.f, 0.f, 0.75f, 1.f));

			int mana = _currentTarget->getMana();
			int maxMana = _currentTarget->getMaxMana();

			float fraction = (float)mana / (float)maxMana;

			std::string str = std::to_string(mana) + std::string{ "/" } +std::to_string(maxMana) + std::string{ " MP" };

			ImGui::ProgressBar(fraction, ImVec2(-1, 20), str.c_str());
			ImGui::PopStyleColor();
		}

		ImGui::End();
	}
	//=========================================================================
	// Render Enemy Nameplate
	//=========================================================================

	// TODO: Always render target on top
	for (auto it : _enemies)
	{
		it->renderUI(_currentTarget == it);
	}
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

void Game::removeFromRoot(
	SceneNode *sceneNode)
{
	_scene->removeChild(sceneNode);
}

Player * Game::getPlayer()
{
	return &_player;
}
