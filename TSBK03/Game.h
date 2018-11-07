#pragma once

#include "Frame.h"

#include "SceneNode.h"
#include "MouseMovedEvent.h"
#include "InputManager.h"
#include "Player.h"

// For collision handling of objects
// http://www.codercorner.com/SAP.pdf
// http://www.peroxide.dk/papers/collision/collision.pdf

struct Enemy
{
	/*
	 * int _unitId;
	 * int _typeId;
	 * Position _position;
	 * Angle _facing;
	 * AiState _aiState;
	 */
};

struct NPC
{
	/*
	 * int _unitId;
	 * int _typeId;
	 * Position _position;
	 * Angle _facing;
	 */
};

struct Zone
{
	/* 
	 * std::vector<NPC> _npcs;
	 * std::vector<Enemy> _enemies;
	 * std::vector<Container> _containers;
	 * std::vector<Trigger> _triggers;
	 *  
	 * // For dynamic loading of terrain from the disk
	 * TerrainChunkManager _terrainChunkManager;
	 * 
	 * WeatherEffect _currentWeather;
	 * 
	 * DirectionalLight _sun; // To be moved to WeatherEffect
	 */
};

struct Buff
{
	/*
	 * int duration;
	 * 
	 * BuffEffect *effect;
	 */
};

// class ItemEquipChangeEvent : public Event
// class BuffChangeEvent : public Event

// class StatManager : public Subscriber<ItemEquipChangeEvent>, public Subscriber<BuffChangeEvent>

// struct Player
// {
	/*
	 * Inventory _inventory;
	 * EquipmentManager _equipManager;
	 * Currency _currency;
	 * Reputation _reputation;
	 * SpellBook _book;
	 * std::vector<Buff> _buffs;
	 * 
	 * 
	 * Race _race;
	 * Class _class;
	 * 
	 * _book.getSpell("fireball").cast(this, target);
	 * 
	 * if(_book.have("fireball"))
	 *		_spellManager.get("fireball").cast(this, target);
	 *		
	 *	Position _position;
	 *	Angle _facing;
	 */
// };

struct Spell
{
	/*
	 * void cast(caster, target);
	 */
};

struct GameState
{
	/*
	 * Zone *_currentZone;
	 * Player _player;
	 * QuestManager _questManager;
	 * AchievementManager _achievementManager;
	 * SpellManager _spellManager;
	 * InputManager _inputManager;
	 * 
	 * ItemDatabase _itemDatabase{"items.db"};
	 * NPCDatabase _npcDatabase{"npcs.db"};
	 * EnemyDatabase _enemyDatabase{"enemies.db"};
	 */
};

class Game : public Frame
{
public:
	explicit Game(Application *application);
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;

	void update(float dt) override;
	void renderUI() override;
	void render(Renderer *renderer) override;

	void addToRoot(SceneNode *sceneNode);
private:

	TerrainSceneNode *_terrainNode;
	Scene *_scene{nullptr};

	InputManager _inputManager;

	Player _player;
};