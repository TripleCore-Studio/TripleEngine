#pragma once

#include <imgui.h>

#include "UIPanel.h"

namespace triple::editor {
	class DescPanel : public UIPanel {
	public:
		DescPanel() : UIPanel("Desc") {}

		void onRender() override {
			ImGuiIO &io = ImGui::GetIO();
			ImGui::SetNextWindowBgAlpha(0.6f);

			ImGui::Begin(m_title.c_str(), nullptr,
			             ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
			                 ImGuiWindowFlags_AlwaysAutoResize);

			float textHeight = ImGui::GetTextLineHeight();
			ImGui::SetCursorPosY((28.0f - textHeight) * 0.5f);

			ImGui::TextDisabled("F9");
			ImGui::SameLine();
			ImGui::Text("Debug");
			ImGui::SameLine();
			ImGui::TextDisabled("|");
			ImGui::SameLine();
			ImGui::TextDisabled("F10");
			ImGui::SameLine();
			ImGui::Text("Mouse");
			ImGui::SameLine();
			ImGui::TextDisabled("|");
			ImGui::SameLine();
			ImGui::TextDisabled("F11");
			ImGui::SameLine();
			ImGui::Text("Fullscreen");
			ImGui::SameLine();
			ImGui::TextDisabled("|");
			ImGui::SameLine();
			ImGui::TextDisabled("WASD");
			ImGui::SameLine();
			ImGui::Text("Move");
			ImGui::SameLine();
			ImGui::TextDisabled("|");
			ImGui::SameLine();
			ImGui::TextDisabled("Space/Shift");
			ImGui::SameLine();
			ImGui::Text("Up/Down");
			ImGui::SameLine();
			ImGui::TextDisabled("|");
			ImGui::SameLine();
			ImGui::TextDisabled("MB4/MB5");
			ImGui::SameLine();
			ImGui::Text("Cam Speed");

			ImGui::SetWindowPos(ImVec2((io.DisplaySize.x - ImGui::GetWindowWidth()) * 0.5f,
			                           io.DisplaySize.y - 38.0f - 10));

			ImGui::End();
		}
	};
} // namespace triple::editor