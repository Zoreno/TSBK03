#pragma once

#include "sol.hpp"

class Game;

class ScriptManager
{
public:

	ScriptManager(const ScriptManager& other) = delete;
	ScriptManager(ScriptManager&& other) = delete;

	ScriptManager& operator=(const ScriptManager& other) = delete;
	ScriptManager& operator=(ScriptManager&& other) = delete;

	ScriptManager(Game *game);

	void doString(const std::string& str);

	void doFile(const std::string& path);

private:

	void registerUserTypes();

	sol::state _luaState;


	Game *_game{ nullptr };
};