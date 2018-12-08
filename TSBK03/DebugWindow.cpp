#include "DebugWindow.h"

#include <iostream>
#include <sstream>

#include "Game.h"
#include "ScriptExecutionException.h"

DebugWindow::DebugWindow(
	Game *game)
	:_game{game}
{

}

void DebugWindow::clearLog()
{
	_items.clear();
	_scrollToBottom = true;
}

void DebugWindow::addToLog(
	const std::string &str)
{
#if 0
	std::stringstream ss{ str };

	do
	{
		std::string s;

		std::getline(ss, s, '\n');

		if (!s.empty())
		{
			_items.push_back(s);

			_scrollToBottom = true;
		}

	} while (!ss.eof());
#else

	if (!str.empty())
	{
		_items.push_back(str);
		_scrollToBottom = true;
	}

#endif

}

void DebugWindow::render(
	bool *pOpen)
{
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Console", pOpen))
	{
		ImGui::End();

		return;
	}

	if (ImGui::Button("Clear"))
	{
		clearLog();
	}

	ImGui::Separator();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	static ImGuiTextFilter filter;
	filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
	ImGui::PopStyleVar();
	ImGui::Separator();

	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText

	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::Selectable("Clear")) clearLog();
		ImGui::EndPopup();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));


	ImVec4 col_default_text = ImGui::GetStyleColorVec4(ImGuiCol_Text);

	for (int i = 0; i < _items.size(); i++)
	{
		const char* item = _items.at(i).c_str();

		if (!filter.PassFilter(item))
		{
			continue;
		}

		ImVec4 col = col_default_text;

		if (strstr(item, "[error]"))
		{
			col = ImColor(1.0f, 0.4f, 0.4f, 1.0f);
		}
		else if (strncmp(item, "# ", 2) == 0)
		{
			col = ImColor(1.0f, 0.78f, 0.58f, 1.0f);
		}

		ImGui::PushStyleColor(ImGuiCol_Text, col);
		ImGui::TextUnformatted(item);
		ImGui::PopStyleColor();
	}

	if (_scrollToBottom)
	{
		ImGui::SetScrollHere(1.0f);
		_scrollToBottom = false;
	}

	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::Separator();

	// Command-line
	bool reclaim_focus = false;
	if (ImGui::InputText("Input", _inputBuffer, IM_ARRAYSIZE(_inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, &textEditCallbackStub, (void*)this))
	{
		char* s = _inputBuffer;

		std::string str{ s };

		executeCommand(s);

		memset(_inputBuffer, 0, INPUT_BUFFER_SIZE);
		reclaim_focus = true;
	}

	// Auto-focus on window apparition
	ImGui::SetItemDefaultFocus();

	if (reclaim_focus)
		ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

	ImGui::End();
}

void DebugWindow::executeCommand(const std::string& str)
{
	addToLog(std::string{ "# " } +str);

	_historyPos = -1;

	for (auto it = _history.begin(); it != _history.end(); ++it)
	{
		if(*it == str)
		{
			_history.erase(it);
			break;
		}
	}

	_history.push_back(str);

	try
	{
		_game->getScriptManager()->doString(str);
	}
	catch (const ScriptExecutionException& ex)
	{
		addToLog(ex.what());
	}
	catch(const sol::error& err)
	{
		std::cerr << err.what() << std::endl;
	}
}

int DebugWindow::textEditCallbackStub(
	ImGuiInputTextCallbackData *data)
{
	DebugWindow *console = (DebugWindow *)data->UserData;

	return console->textEditCallback(data);
}

int DebugWindow::textEditCallback(
	ImGuiInputTextCallbackData *data)
{
	switch(data->EventFlag)
	{
	case ImGuiInputTextFlags_CallbackHistory:
	{
		const int prevHistPos = _historyPos;

		if(data->EventKey == ImGuiKey_UpArrow)
		{
			if(_historyPos == -1)
			{
				_historyPos = _history.size() - 1;
			}
			else if(_historyPos > 0)
			{
				--_historyPos;
			}
		}
		else if(data->EventKey == ImGuiKey_DownArrow)
		{
			if(_historyPos != -1)
			{
				if(++_historyPos >= _history.size())
				{
					_historyPos = -1;
				}
			}
		}

		if(prevHistPos != _historyPos)
		{
			const char *histStr = (_historyPos >= 0) ? _history.at(_historyPos).c_str() : "";
			data->DeleteChars(0, data->BufTextLen);
			data->InsertChars(0, histStr);
		}
	}
	break;
	}

	return 0;
}
