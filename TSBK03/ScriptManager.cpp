#include "ScriptManager.h"

#include "Player.h"

#include <glm/glm.hpp>

#include "ScriptExecutionException.h"

#include "Utils.h"

#include "Game.h"

static std::string get_as_string(sol::state& lua, sol::object o)
{
	return lua["tostring"](o);
};


ScriptManager::ScriptManager(Game *game)
	:_game{ game }
{
	_luaState.open_libraries(sol::lib::base);

	registerUserTypes();

	_luaState["game"] = _game;
	_luaState["player"] = _game->getPlayer();
	_luaState["scriptManager"] = _game->getScriptManager();

	sol::state& state = _luaState;
	
	_luaState.set_function("print", [game, &state](sol::variadic_args va)
	{
		std::string line;

		bool first = true;

		for(const auto& v : va)
		{
			std::string s = get_as_string(state, v.get<sol::object>());

			line += std::string{ first ? "" : "\t" } +s;

			first = false;
		}

		game->getDebugWindow()->addToLog(line);
	});

	std::cout << "LUA initialized" << std::endl;
}

void ScriptManager::doString(
	const std::string &str)
{
	sol::load_result loadresult = _luaState.load(str);

	if (!loadresult.valid())
	{
		sol::error err = loadresult;
		std::string what = err.what();

		throw ScriptExecutionException{ std::string{"[error] "} + what };
	}


	sol::protected_function script_func = loadresult;
	sol::protected_function_result result = script_func();

	if (!result.valid())
	{
		sol::error err = result;
		std::string what = err.what();

		throw ScriptExecutionException{ std::string{ "[error] " } + what };
	}
}

void ScriptManager::doFile(
	const std::string &path)
{
	auto result = _luaState.script_file(path, [](lua_State*, sol::protected_function_result pfr)
	{
		return pfr;
	});

	if (!result.valid())
	{
		sol::error err = result;
		std::string what = err.what();

		throw ScriptExecutionException{ std::string{ "[error] " } + what };
	}
}

#define API_FUNCTION(name) #name,&T::name
#define API_MEMBER(name) #name,&T::name

void ScriptManager::registerUserTypes()
{
	{
		using T = Player;

		_luaState.new_usertype<T>("Player",
			API_FUNCTION(getPosition),
			API_FUNCTION(setPosition),

			API_FUNCTION(getFacing),
			API_FUNCTION(setFacing),

			API_FUNCTION(getSpeed),
			API_FUNCTION(setSpeed),

			API_FUNCTION(getTouchingGround),
			API_FUNCTION(getWalking),

			API_FUNCTION(getName),

			API_FUNCTION(getLevel),

			API_FUNCTION(getExperience),

			API_FUNCTION(getExperienceToLevel),

			API_FUNCTION(getHealth),
			API_FUNCTION(setHealth),

			API_FUNCTION(getMaxHealth),

			API_FUNCTION(getMissingHealth),

			API_FUNCTION(getMana),

			API_FUNCTION(getMaxMana),

			API_FUNCTION(addExperience),

			API_FUNCTION(takeDamage),

			API_FUNCTION(heal),

			API_FUNCTION(recalculateStats),

			API_FUNCTION(getHealthPer5),

			API_FUNCTION(getManaPer5),

			API_FUNCTION(getCritChance),

			API_FUNCTION(getInventory),

			API_FUNCTION(getStats),
			API_FUNCTION(getBaseStats)

			);
	}

	{
		using T = Game;

		_luaState.new_usertype<T>("Game",
			API_FUNCTION(getPlayer),
			API_FUNCTION(spawnEnemy),
			API_FUNCTION(getScriptManager),
			API_FUNCTION(getApplication),
			API_FUNCTION(getEnemies),
			API_FUNCTION(getCurrentTarget)

			);
	}

	{
		using T = Enemy;

		_luaState.new_usertype<T>("Enemy",
			API_FUNCTION(getPosition),
			API_FUNCTION(setPosition),
			API_FUNCTION(getHealth),
			API_FUNCTION(getMaxHealth),
			API_FUNCTION(getMana),
			API_FUNCTION(getMaxMana),
			API_FUNCTION(getName),
			API_FUNCTION(getLevel),
			API_FUNCTION(takeDamage),
			API_FUNCTION(distanceToPlayer),
			API_FUNCTION(distanceToStart)

			);
	}

	{
		using T = Inventory;

		_luaState.new_usertype<T>("Inventory",
			API_FUNCTION(getSize),

			API_FUNCTION(getItemAt),

			API_FUNCTION(addItem),
			API_FUNCTION(addItemCount),
			API_FUNCTION(getItemCount),

			API_FUNCTION(removeItem),
			API_FUNCTION(removeItemAt),
			API_FUNCTION(removeItemCountAt)

			);
	}

	{
		using T = ItemInstance;

		_luaState.new_usertype<T>("ItemInstance",
			sol::constructors<T(unsigned int, unsigned int)>(),
			API_FUNCTION(getID),
			API_FUNCTION(setID),
			API_FUNCTION(getStackSize),
			API_FUNCTION(setStackSize)
			);
	}

	{
		using T = Stats;

		_luaState.new_usertype<T>("Stats",
			API_MEMBER(agility),
			API_MEMBER(strength),
			API_MEMBER(stamina),
			API_MEMBER(intellect),
			API_MEMBER(spirit)
			);
	}

	{
		using T = ScriptManager;

		_luaState.new_usertype<T>("ScriptManager",
			API_FUNCTION(doFile)
			);
	}

	{
		using T = glm::vec2;

		_luaState.new_usertype<T>("vec2",

			/*
			* Constructors
			*/

			sol::constructors<T(), T(float), T(const T&), T(float, float)>(),

			/*
			* Element access
			*/

			API_MEMBER(x),
			API_MEMBER(y),

			/*
			* Operators
			*/

			sol::meta_function::addition, sol::resolve<T(const T&, const T&)>(glm::operator+),
			sol::meta_function::subtraction, sol::resolve<T(const T&, const T&)>(glm::operator-),
			sol::meta_function::multiplication, sol::resolve<T(const T&, const T&)>(glm::operator*),
			sol::meta_function::division, sol::resolve<T(const T&, const T&)>(glm::operator/)

			);
	}

	{
		using T = glm::vec3;

		_luaState.new_usertype<T>("vec3",

			/*
			 * Constructors
			 */

			sol::constructors<T(), T(float), T(const T&), T(float, float, float)>(),

			/*
			* Element access
			*/

			API_MEMBER(x),
			API_MEMBER(y),
			API_MEMBER(z),

			/*
			* Operators
			*/

			sol::meta_function::addition, sol::resolve<T(const T&, const T&)>(glm::operator+),
			sol::meta_function::subtraction, sol::resolve<T(const T&, const T&)>(glm::operator-),
			sol::meta_function::multiplication, sol::resolve<T(const T&, const T&)>(glm::operator*),
			sol::meta_function::division, sol::resolve<T(const T&, const T&)>(glm::operator/)

			);
	}

	{
		using T = glm::vec4;

		_luaState.new_usertype<T>("vec4",

			/*
			* Constructors
			*/

			sol::constructors<T(), T(float), T(const T&), T(float, float, float, float)>(),

			/*
			* Element access
			*/
			API_MEMBER(x),
			API_MEMBER(y),
			API_MEMBER(z),
			API_MEMBER(w),

			/*
			* Operators
			*/

			sol::meta_function::addition, sol::resolve<T(const T&, const T&)>(glm::operator+),
			sol::meta_function::subtraction, sol::resolve<T(const T&, const T&)>(glm::operator-),
			sol::meta_function::multiplication, sol::resolve<T(const T&, const T&)>(glm::operator*),
			sol::meta_function::division, sol::resolve<T(const T&, const T&)>(glm::operator/)

			);
	}
}
