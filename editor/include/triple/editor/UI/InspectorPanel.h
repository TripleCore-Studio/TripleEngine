#pragma once

#include <imgui.h>

#include <triple/game/ecs/Scene.h>
#include <triple/game/ecs/TransformComponent.h>

#include <triple/editor/SunLight.h>

#include "UIPanel.h"

namespace triple::editor {
	class InspectorPanel : public UIPanel {
	public:
		InspectorPanel() : UIPanel("Inspector") {}

		void setTarget(entt::entity entity, game::Scene *scene) {
			m_entity = entity;
			m_scene = scene;
		}

		void onRender() override {
			ImGuiIO &io = ImGui::GetIO();
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 10, 10), ImGuiCond_Always,
			                        ImVec2(1.0f, 0.0f));

			ImGui::Begin(m_title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize);

			if (m_scene) {
				auto *t = m_scene->getRegistry().try_get<game::TransformComponent>(m_entity);
				if (t) {
					ImGui::Text("Transform");
					ImGui::DragFloat3("Position", &t->position.x, 0.05f);
					ImGui::DragFloat3("Rotation", &t->rotationEuler.x, 1.0f);
					ImGui::DragFloat3("Scale", &t->scale.x, 0.05f);
				}
			}

			if (m_cameraSpeed) {
				ImGui::Separator();
				ImGui::Text("Camera");
				ImGui::SliderFloat("Speed", m_cameraSpeed, m_cameraSpeedMin, m_cameraSpeedMax);
				if (m_lockY)
					ImGui::Checkbox("Lock Y", m_lockY);
			}

			ImGui::End();
		}

		void setCamera(float *speed, float min, float max, bool *lockY) {
			m_cameraSpeed = speed;
			m_cameraSpeedMin = min;
			m_cameraSpeedMax = max;
			m_lockY = lockY;
		}

	private:
		entt::entity m_entity = entt::null;
		game::Scene *m_scene = nullptr;

		float *m_cameraSpeed = nullptr;
		float m_cameraSpeedMin = 1.0f;
		float m_cameraSpeedMax = 1000.0f;
		bool *m_lockY = nullptr;

		editor::SunLight *m_sunLight = nullptr;
		math::Vec3 *m_ambientColor = nullptr;
	};
} // namespace triple::editor