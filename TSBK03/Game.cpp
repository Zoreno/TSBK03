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

	_directionalLightNode = new DirectionalLightSceneNode{
		DirectionalLight{
		glm::vec3{ 3.f, -3.f, 3.f },
		glm::vec3{ 0.2f, 0.2f, 0.2f },
		glm::vec3{ 0.98f, 0.98f, 0.90f },
		glm::vec3{ 0.8f, 0.8f, 0.8f } },
		"DirectionalLight1" };

	_scene->addChild(_directionalLightNode);

	application->getEventManager()->addSubscriber<MouseMovedEvent>(&_inputManager);
	application->getEventManager()->addSubscriber<MouseButtonEvent>(&_inputManager);
	application->getEventManager()->addSubscriber<KeyEvent>(&_inputManager);

	Terrain *terrain = _application->getAssetManager()->fetch<Terrain>(_terrainNode->getTerrain());

	_enemies.push_back(new Enemy{ 0, this , glm::vec3{ 200.f, 0.f, 220.f }, terrain });
	_enemies.push_back(new Enemy{ 1, this , glm::vec3{ 240.f, 0.f, 220.f }, terrain });

	_player.getInventory()->addItem(ItemInstance(0, 1));
	_player.getInventory()->addItem(ItemInstance(0, 1));
	_player.getInventory()->addItem(ItemInstance(1, 1));
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

	static float currTime = 0;
	currTime += dt;

	_directionalLightNode->getDirectionalLight().setDirection(
		3.f * glm::vec3{ glm::cos(currTime / 10.f), 0 ? -0.5f - 0.5f * glm::cos(currTime / 20.f) : -1.f, glm::sin(currTime / 10.f) });

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
				if (_currentTarget != nullptr)
				{
					_currentTarget->onUntargeted();
				}

				_currentTarget = it;

				_currentTarget->onTargeted();

				found = true;

				break;
			}
		}

		if (!found && _currentTarget != nullptr)
		{
			_currentTarget->onUntargeted();
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
				_currentTarget->onUntargeted();
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

	if (_player.getHealth() <= 0)
	{
		// TODO: Do something else
		// We can move the player to a starting point or a checkpoint set by
		// the player
		_application->shutDown();
	}
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
	// Render Inventory Screen
	//=========================================================================

	static bool showInventory = true;
	// TODO: Move all this to a UI Manager
	if (showInventory)
	{
		ImGui::Begin("Inventory", &showInventory, ImVec2(0.f, 0.f), 0.9f,
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

		Inventory *playerInventory = _player.getInventory();
		Texture2D *iconTexture = _application->getAssetManager()->fetch<Texture2D>("icons");

		int moveFrom = -1;
		int moveTo = -1;

		for (unsigned int i = 0; i < playerInventory->getSize(); ++i)
		{
			ImGui::PushID(i);

			float imageWidth = 1.f;
			float imageHeight = 1.f;

			float iconWidth = imageWidth / 28.f;
			float iconHeight = imageHeight / 29.f;

			ItemInstance& itemInstance = playerInventory->getItemAt(i);
			Item *item = _itemDatabase.getItemById(itemInstance.getID());

			if (itemInstance.getID() != INVALID_ID)
			{
				int x = item->getIconX();
				int y = item->getIconY();

				if (ImGui::ImageButton(
					(void *)(intptr_t)iconTexture->getHandle(),
					ImVec2(32.f, 32.f),
					ImVec2(iconWidth * x, imageHeight - y * iconHeight),
					ImVec2(iconWidth * (x + 1), imageHeight - (y + 1) * iconHeight),
					2))
				{

				}

				ImGuiDragDropFlags srcFlags = 0;
				srcFlags |= ImGuiDragDropFlags_SourceNoDisableHover;
				srcFlags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers;

				if (ImGui::BeginDragDropSource(srcFlags))
				{
					ImGui::SetDragDropPayload("DND_DEMO_NAME", &i, sizeof(int));
					ImGui::EndDragDropSource();
				}

				if (ImGui::BeginDragDropTarget())
				{
					ImGuiDragDropFlags targetFlags = 0;

					if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("DND_DEMO_NAME", targetFlags))
					{
						moveFrom = *(const int *)payload->Data;
						moveTo = i;
					}

					ImGui::EndDragDropTarget();
				}

				if (ImGui::IsItemClicked(1))
				{
					ImGui::OpenPopup("Menu");
				}

				if (ImGui::IsItemHovered())
				{
					if (ImGui::Begin("1", NULL, ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
					{
						ImVec4 color = ImVec4{ 1.f, 1.f, 1.f, 1.f };

						switch (item->getRarity())
						{
						case ItemRarity::UNCOMMON:
							color = ImVec4{ 0.25f, 0.91f, 0.16f, 1.0f };
							break;
						case ItemRarity::SUPERIOR:
							color = ImVec4{ 0.09f, 0.17f, 0.78f, 1.0f };
							break;
						case ItemRarity::EPIC:
							color = ImVec4{ 0.64f, 0.21f, 0.93f, 1.0f };
							break;
						case ItemRarity::LEGENDARY:
							color = ImVec4{ 0.99f, 0.40f, 0.f, 1.0f };
							break;
						}

						ImGui::TextColored(color, item->getName().c_str());
						ImGui::TextColored(ImVec4{ 1.f, 0.82f, 0.07f, 1.0f }, item->getDescription().c_str());
						ImGui::End();
					}
				}

				if (ImGui::BeginPopup("Menu"))
				{
					if (ImGui::MenuItem("Use"))
					{
						std::cout << "Use clicked" << std::endl;
					}

					if (ImGui::MenuItem("Destroy"))
					{
						std::cout << "Destroy clicked" << std::endl;
						playerInventory->removeItemAt(i);
					}

					if (ImGui::MenuItem("Sell"))
					{
						std::cout << "Sell clicked" << std::endl;
					}

					if (ImGui::MenuItem("Cancel"))
					{
						std::cout << "Cancel clicked" << std::endl;
					}

					ImGui::EndPopup();
				}
			}
			else
			{
				int x = 4;
				int y = 28;

				ImGui::ImageButton(
					(void *)(intptr_t)iconTexture->getHandle(),
					ImVec2(32.f, 32.f),
					ImVec2(iconWidth * x, imageHeight - y * iconHeight),
					ImVec2(iconWidth * (x + 1), imageHeight - (y + 1) * iconHeight),
					2);

				if (ImGui::BeginDragDropTarget())
				{
					ImGuiDragDropFlags targetFlags = 0;

					if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("DND_DEMO_NAME", targetFlags))
					{
						moveFrom = *(const int *)payload->Data;
						moveTo = i;
					}

					ImGui::EndDragDropTarget();
				}

			}

			if (i % 4 != 3)
			{
				ImGui::SameLine();
			}

			ImGui::PopID();
		}

		if (moveFrom != -1 && moveTo != -1)
		{
			playerInventory->swapItems(moveFrom, moveTo);
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

ItemDatabase * Game::getItemDatabase()
{
	return &_itemDatabase;
}
