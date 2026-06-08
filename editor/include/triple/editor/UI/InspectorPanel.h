#pragma once

#include <imgui.h>

#include <triple/game/ecs/Scene.h>
#include <triple/game/ecs/TransformComponent.h>

#include <triple/gfx/LightSources.h>
#include <triple/editor/SunLight.h>

#include "UIPanel.h"

namespace triple::editor {
	class InspectorPanel : public UIPanel {
	public:
		InspectorPanel() : UIPanel("Inspector") {}

		void setTarget(game::Entity entity, game::Scene *scene) {
			m_entity = entity;
			m_scene = scene;
		}

		void setLight(editor::SunLight *sunLight, gfx::CameraLight *cameraLight,
		              math::Vec3 *ambientColor) {
			m_sunLight = sunLight;
			m_cameraLight = cameraLight;
			m_ambientColor = ambientColor;
		}

		void onRender() override {
			ImGuiIO &io = ImGui::GetIO();
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 10, 10), ImGuiCond_Always,
			                        ImVec2(1.0f, 0.0f));

			ImGui::Begin(m_title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize);

			if (m_scene) {
				auto *t = m_scene->getComponent<game::TransformComponent>(m_entity);
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

			ImGui::Separator();
			ImGui::Text("Ambient");

			if (m_ambientColor) {
				ImGui::ColorEdit3("Ambient Color", &m_ambientColor->x);
			}

			if (m_sunLight) {
				ImGui::Separator();
				ImGui::Text("Sun");

				ImGui::ColorEdit3("Sun Color", &m_sunLight->color.x);
				ImGui::SliderFloat("Sun Intensity", &m_sunLight->intensity, 0.0f, 10.0f);
				ImGui::SliderFloat("Elevation", &m_sunLight->elevation, 0.0f, 90.0f);
				ImGui::SliderFloat("Azimuth", &m_sunLight->azimuth, 0.0f, 360.0f);
			}

			if (m_cameraLight) {
				ImGui::Separator();
				ImGui::Text("Camera Light");

				ImGui::ColorEdit3("Cam Color", &m_cameraLight->color.x);
				ImGui::SliderFloat("Cam Intensity", &m_cameraLight->intensity, 0.0f, 20.0f);
				ImGui::SliderFloat("Cam Radius", &m_cameraLight->radius, 0.0f, 100.0f);
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
		game::Entity m_entity = {};
		game::Scene *m_scene = nullptr;

		float *m_cameraSpeed = nullptr;
		float m_cameraSpeedMin = 1.0f;
		float m_cameraSpeedMax = 1000.0f;
		bool *m_lockY = nullptr;

		editor::SunLight *m_sunLight = nullptr;
		gfx::CameraLight *m_cameraLight = nullptr;
		math::Vec3 *m_ambientColor = nullptr;
	};
} // namespace triple::editor