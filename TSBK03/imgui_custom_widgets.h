#pragma once

#include "imgui.h"

namespace ImGui
{
	IMGUI_API void ImageWithText(
		ImTextureID user_texture_id, 
		const ImVec2& size, 
		const ImVec2& uv0 = ImVec2(0, 0), 
		const ImVec2& uv1 = ImVec2(1, 1), 
		const ImVec4& tint_col = ImVec4(1, 1, 1, 1), 
		const ImVec4& border_col = ImVec4(0, 0, 0, 0),
		const ImVec4& text_col = ImVec4(1, 1, 1, 1),
		const ImVec2& offset = ImVec2(0, 0), 
		const char *text = "",
		const ImVec4& fill_color = ImVec4(0, 0, 0, 0),
		const float fill_percentage = 0.f);

	IMGUI_API bool ImageButtonWithText(
		ImTextureID user_texture_id, 
		const ImVec2& size, 
		const ImVec2& uv0 = ImVec2(0, 0), 
		const ImVec2& uv1 = ImVec2(1, 1), 
		int frame_padding = -1, 
		const ImVec4& bg_col = ImVec4(0, 0, 0, 0), 
		const ImVec4& tint_col = ImVec4(1, 1, 1, 1), 
		const ImVec4& text_col = ImVec4(1, 1, 1, 1),
		const ImVec2& offset = ImVec2(0, 0),
		const char *text = "",
		const ImVec4& fill_color = ImVec4(0, 0, 0, 0),
		const float fill_percentage = 0.f);
}