#pragma once

#include <functional>

#include <imgui.h>
#include <ImGuiFileDialog.h>

#include <triple/game/ecs/Scene.h>
#include <triple/game/ecs/NameComponent.h>

#include "UIPanel.h"

namespace triple::editor {
	class HierarchyPanel : public UIPanel {
	public:
		static constexpr int POS_X = 10;
		static constexpr int POS_Y = 10;

		static constexpr int WIDTH = 250;
		static constexpr int HEIGHT = 400;

		explicit HierarchyPanel(game::Scene *scene,
		                        std::function<void(entt::entity)> entitySelected,
		                        std::function<void(std::string modelPath)> addEntity,
		                        std::function<void(entt::entity)> removeEntity,
		                        std::function<void(entt::entity)> duplicate)
		    : UIPanel("Hierarchy"), m_scene(scene), m_onEntitySelected(entitySelected),
		      m_onAddEntity(addEntity), m_onRemoveEntity(removeEntity), m_onDuplicate(duplicate) {}

		void onRender() override {
			auto &registry = m_scene->getRegistry();

			ImGuiIO &io = ImGui::GetIO();
			ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
			ImGui::SetNextWindowSize(ImVec2(260, 400), ImGuiCond_FirstUseEver);

			ImGui::Begin(m_title.c_str());

			auto view = registry.view<game::NameComponent>();
			for (auto [entity, name] : view.each()) {
				bool selected = (m_selected == entity);

				ImGui::PushID(static_cast<int>(entt::to_integral(entity)));

				if (ImGui::Selectable(name.name.c_str(), selected)) {
					m_selected = entity;
					if (m_onEntitySelected)
						m_onEntitySelected(entity);
				}

				if (ImGui::BeginPopupContextItem()) {
					if (ImGui::MenuItem("Duplicate")) {
						if (m_onDuplicate)
							m_onDuplicate(entity);
					}
					ImGui::Separator();
					if (ImGui::MenuItem("Remove")) {
						if (m_onRemoveEntity)
							m_onRemoveEntity(entity);
					}
					ImGui::EndPopup();
				}

				ImGui::PopID();
			}

			float buttonHeight = ImGui::GetFrameHeightWithSpacing();
			float buttonWidth = 120.0f;

			ImGui::SetCursorPosY(ImGui::GetWindowHeight() - buttonHeight -
			                     ImGui::GetStyle().WindowPadding.y - 10);
			ImGui::Separator();

			ImGui::SetCursorPosY(ImGui::GetWindowHeight() - buttonHeight -
			                     ImGui::GetStyle().WindowPadding.y);

			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - buttonWidth -
			                     ImGui::GetStyle().WindowPadding.x);

			if (ImGui::Button("+ Add", ImVec2(buttonWidth, 0))) {
				IGFD::FileDialogConfig config;
				config.flags = ImGuiFileDialogFlags_Modal;
				ImGuiFileDialog::Instance()->OpenDialog("ChooseModel", "Choose Model", ".glb",
				                                        config);
			}

			if (ImGuiFileDialog::Instance()->IsOpened("ChooseModel")) {
				ImVec2 size(io.DisplaySize.x * 0.33f, io.DisplaySize.y * 0.4f);
				ImGui::SetNextWindowPos(
				    ImVec2((io.DisplaySize.x - size.x) * 0.5f, (io.DisplaySize.y - size.y) * 0.5f),
				    ImGuiCond_Always);
				ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);

				if (ImGuiFileDialog::Instance()->Display("ChooseModel")) {
					if (ImGuiFileDialog::Instance()->IsOk()) {
						std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
						if (m_onAddEntity)
							m_onAddEntity(path);
					}
					ImGuiFileDialog::Instance()->Close();
				}
			}

			ImGui::End();
		}

	private:
		game::Scene *m_scene;
		entt::entity m_selected;

		std::function<void(entt::entity)> m_onEntitySelected;
		std::function<void(std::string modelPath)> m_onAddEntity;
		std::function<void(entt::entity)> m_onRemoveEntity;
		std::function<void(entt::entity)> m_onDuplicate;
	};
} // namespace triple::editor