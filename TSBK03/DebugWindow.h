#pragma once

#include "imgui.h"

#include <vector>
#include <string>

class Game;

#define INPUT_BUFFER_SIZE 256

class DebugWindow
{
public:
	explicit DebugWindow(Game *game);

	void clearLog();

	void addToLog(const std::string& str);

	void render(bool *pOpen);

	void executeCommand(const std::string& str);

private:
	static int textEditCallbackStub(ImGuiInputTextCallbackData *data);
	int textEditCallback(ImGuiInputTextCallbackData *data);

	char _inputBuffer[INPUT_BUFFER_SIZE];
	std::vector<std::string> _items;
	bool _scrollToBottom = true;
	std::vector<std::string> _history;
	int _historyPos = -1;

	Game *_game;
};
