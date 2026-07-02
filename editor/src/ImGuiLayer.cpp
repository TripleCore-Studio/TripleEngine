#include "triple/editor/UI/ImGuiLayer.h"

#include <filesystem>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <ImGuiFileDialog.h>

#include <triple/core/event/KeyboardInputEvent.h>
#include <triple/core/event/MouseMoveEvent.h>
#include <triple/core/event/MouseButtonEvent.h>
#include <triple/core/event/WindowResizeEvent.h>
#include <triple/core/event/TextInputEvent.h>

#include <triple/core/input/KeyCode.h>
#include <triple/core/input/MouseButton.h>

#include <triple/game/utils/HierarchyUtils.h>
#include <triple/game/ecs/NameComponent.h>
#include <triple/game/ecs/MeshComponent.h>
#include <triple/game/ecs/TransformComponent.h>

#include <triple/game/asset/Model.h>
#include <triple/game/asset/AssetTypes.h>

#include "triple/editor/UI/HierarchyPanel.h"
#include "triple/editor/UI/DescPanel.h"

using namespace triple::math;

namespace {
	std::string uniqueName(entt::registry &registry, const std::string &baseName) {
		std::string name = baseName;
		int counter = 1;
		while (true) {
			bool found = false;
			auto view = registry.view<triple::game::NameComponent>();
			for (auto [entity, n] : view.each()) {
				if (n.name == name) {
					found = true;
					break;
				}
			}
			if (!found)
				return name;
			name = baseName + "(" + std::to_string(counter++) + ")";
		}
	}
} // namespace

namespace triple::editor {
	ImGuiKey toImGuiKey(core::KeyCode key) {
		using namespace triple::core;
		const int k = static_cast<int>(key);

		if (k >= static_cast<int>(KeyCode::A) && k <= static_cast<int>(KeyCode::Z))
			return static_cast<ImGuiKey>(ImGuiKey_A + (k - static_cast<int>(KeyCode::A)));
		if (k >= static_cast<int>(KeyCode::Num0) && k <= static_cast<int>(KeyCode::Num9))
			return static_cast<ImGuiKey>(ImGuiKey_0 + (k - static_cast<int>(KeyCode::Num0)));
		if (k >= static_cast<int>(KeyCode::F1) && k <= static_cast<int>(KeyCode::F24))
			return static_cast<ImGuiKey>(ImGuiKey_F1 + (k - static_cast<int>(KeyCode::F1)));
		if (k >= static_cast<int>(KeyCode::KP0) && k <= static_cast<int>(KeyCode::KP9))
			return static_cast<ImGuiKey>(ImGuiKey_Keypad0 + (k - static_cast<int>(KeyCode::KP0)));

		switch (key) {
			case KeyCode::Space:
				return ImGuiKey_Space;
			case KeyCode::Tab:
				return ImGuiKey_Tab;
			case KeyCode::Backspace:
				return ImGuiKey_Backspace;
			case KeyCode::Enter:
				return ImGuiKey_Enter;
			case KeyCode::Escape:
				return ImGuiKey_Escape;
			case KeyCode::Left:
				return ImGuiKey_LeftArrow;
			case KeyCode::Right:
				return ImGuiKey_RightArrow;
			case KeyCode::Up:
				return ImGuiKey_UpArrow;
			case KeyCode::Down:
				return ImGuiKey_DownArrow;
			case KeyCode::Insert:
				return ImGuiKey_Insert;
			case KeyCode::Delete:
				return ImGuiKey_Delete;
			case KeyCode::Home:
				return ImGuiKey_Home;
			case KeyCode::End:
				return ImGuiKey_End;
			case KeyCode::PageUp:
				return ImGuiKey_PageUp;
			case KeyCode::PageDown:
				return ImGuiKey_PageDown;
			case KeyCode::LeftShift:
				return ImGuiKey_LeftShift;
			case KeyCode::RightShift:
				return ImGuiKey_RightShift;
			case KeyCode::LeftCtrl:
				return ImGuiKey_LeftCtrl;
			case KeyCode::RightCtrl:
				return ImGuiKey_RightCtrl;
			case KeyCode::LeftAlt:
				return ImGuiKey_LeftAlt;
			case KeyCode::RightAlt:
				return ImGuiKey_RightAlt;
			case KeyCode::LeftSuper:
				return ImGuiKey_LeftSuper;
			case KeyCode::RightSuper:
				return ImGuiKey_RightSuper;
			case KeyCode::CapsLock:
				return ImGuiKey_CapsLock;
			case KeyCode::ScrollLock:
				return ImGuiKey_ScrollLock;
			case KeyCode::NumLock:
				return ImGuiKey_NumLock;
			case KeyCode::PrintScreen:
				return ImGuiKey_PrintScreen;
			case KeyCode::Pause:
				return ImGuiKey_Pause;
			case KeyCode::Menu:
				return ImGuiKey_Menu;
			case KeyCode::Apostrophe:
				return ImGuiKey_Apostrophe;
			case KeyCode::Comma:
				return ImGuiKey_Comma;
			case KeyCode::Minus:
				return ImGuiKey_Minus;
			case KeyCode::Period:
				return ImGuiKey_Period;
			case KeyCode::Slash:
				return ImGuiKey_Slash;
			case KeyCode::Semicolon:
				return ImGuiKey_Semicolon;
			case KeyCode::Equal:
				return ImGuiKey_Equal;
			case KeyCode::LeftBracket:
				return ImGuiKey_LeftBracket;
			case KeyCode::Backslash:
				return ImGuiKey_Backslash;
			case KeyCode::RightBracket:
				return ImGuiKey_RightBracket;
			case KeyCode::GraveAccent:
				return ImGuiKey_GraveAccent;
			case KeyCode::KPDecimal:
				return ImGuiKey_KeypadDecimal;
			case KeyCode::KPDivide:
				return ImGuiKey_KeypadDivide;
			case KeyCode::KPMultiply:
				return ImGuiKey_KeypadMultiply;
			case KeyCode::KPSubtract:
				return ImGuiKey_KeypadSubtract;
			case KeyCode::KPAdd:
				return ImGuiKey_KeypadAdd;
			case KeyCode::KPEnter:
				return ImGuiKey_KeypadEnter;
			case KeyCode::KPEqual:
				return ImGuiKey_KeypadEqual;
			default:
				return ImGuiKey_None;
		}
	}

	int toImGuiButton(core::MouseButton button) {
		using namespace triple::core;

		switch (button) {
			case MouseButton::Left:
				return 0;
			case MouseButton::Right:
				return 1;
			case MouseButton::Middle:
				return 2;
			case MouseButton::Button4:
				return 3;
			case MouseButton::Button5:
				return 4;
			default:
				return -1;
		}
	}

	void ImGuiLayer::onAttach(const core::EngineContext &ctx) {
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		if (!ImGui_ImplOpenGL3_Init("#version 330")) {
			m_initialized = false;
			return;
		}
		configureApply();
		stylesApply();
		resize(ctx.window->getWidth(), ctx.window->getHeight());
		load();
		m_initialized = true;
	}

	void ImGuiLayer::show() {
		m_uiManager.showAll();
		opened = true;
	}

	void ImGuiLayer::hide() {
		m_uiManager.hideAll();
		opened = false;
	}

	void ImGuiLayer::load() {
		m_uiManager.addPanel<HierarchyPanel>(
		    m_gameLayer->getActiveScene(),
		    [this](entt::entity e) { m_inspector->setTarget(e, m_gameLayer->getActiveScene()); },
		    [this](std::string path) {
			    entt::registry &registry = m_gameLayer->getActiveScene()->getRegistry();

			    entt::entity entity = registry.create();
			    std::string baseName = std::filesystem::path(path).stem().string();
			    std::string name = uniqueName(registry, baseName);

			    registry.emplace<game::NameComponent>(entity, name);
			    registry.emplace<game::TransformComponent>(entity);

			    if (!m_loadedModels->empty()) {
				    for (auto &modelStr : *m_loadedModels) {
					    game::TypedAssetID<game::Model> modelId =
					        m_gameLayer->getAssetManager()->storageFor<game::Model>().findByName(
					            modelStr);
					    if (modelId.isValid()) {
						    registry.emplace<game::MeshComponent>(entity, modelId);
						    break; // only the first valid model is attached
					    }
				    }
			    } else {
				    std::optional<game::TypedAssetID<game::Model>> modelId =
				        m_gameLayer->getAssetManager()->load<game::Model>(
				            baseName, game::ModelLoadParams{path});
				    if (modelId) {
					    registry.emplace<game::MeshComponent>(entity, *modelId);
				    }
			    }
		    },
		    [this](entt::entity e) {
			    if (e != m_gameLayer->getActiveCamera()) {
				    game::HierarchyUtils::destroyEntityRecursive(
				        m_gameLayer->getActiveScene()->getRegistry(), e);
			    }
		    },
		    [this](entt::entity e) {
			    if (e == m_gameLayer->getActiveCamera())
				    return;

			    entt::registry &registry = m_gameLayer->getActiveScene()->getRegistry();

			    game::TransformComponent currentTransform =
			        registry.get<game::TransformComponent>(e);
			    game::MeshComponent currentMesh = registry.get<game::MeshComponent>(e);
			    game::NameComponent currentName = registry.get<game::NameComponent>(e);

			    std::string newName = uniqueName(registry, currentName.name);

			    entt::entity newEntity = registry.create();
			    game::TransformComponent &newTransform =
			        registry.emplace<game::TransformComponent>(newEntity);

			    newTransform.position = currentTransform.position + Vec3(1, 1, 1);
			    newTransform.rotationEuler = currentTransform.rotationEuler;
			    newTransform.scale = currentTransform.scale;

			    registry.emplace<game::MeshComponent>(newEntity, currentMesh.model);
			    registry.emplace<game::NameComponent>(newEntity, newName);
		    });

		m_inspector = m_uiManager.addPanel<InspectorPanel>();
		m_inspector->setCamera(
		    &m_gameLayer->cameraSettings.cameraSpeed, m_gameLayer->cameraSettings.cameraSpeedMin,
		    m_gameLayer->cameraSettings.cameraSpeedMax, &m_gameLayer->cameraSettings.lockY);
		m_inspector->setLight(m_sunLight, m_cameraLight, m_ambientColor);

		m_uiManager.addPanel<DescPanel>();

		if (!opened) {
			m_uiManager.hideAll();
		}
	}

	ImGuiLayer::~ImGuiLayer() { onDetach(); }

	void ImGuiLayer::onDetach() {
		if (m_initialized) {
			ImGui_ImplOpenGL3_Shutdown();
			ImGui::DestroyContext();
			m_initialized = false;
		}
	}

	void ImGuiLayer::beginFrame() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::endFrame() {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiLayer::onUpdate(float dt) {
		ImGuiIO &io = ImGui::GetIO();
		io.DeltaTime = dt;
	}

	void ImGuiLayer::onEvent(core::Event &e) {
		if (!opened)
			return;

		ImGuiIO &io = ImGui::GetIO();

		if (e.getTypeID() == core::KeyboardInputEvent::staticTypeID()) {
			auto &key = static_cast<core::KeyboardInputEvent &>(e);
			io.AddKeyEvent(toImGuiKey(key.getKey()), key.isPressed() || key.isRepeat());
			if (key.isPressed() && ImGui::GetIO().WantCaptureKeyboard)
				e.handled = true;
		} else if (e.getTypeID() == core::MouseMoveEvent::staticTypeID()) {
			auto &move = static_cast<core::MouseMoveEvent &>(e);
			io.AddMousePosEvent(move.getX(), move.getY());
			e.handled = true;
		} else if (e.getTypeID() == core::MouseButtonEvent::staticTypeID()) {
			auto &btn = static_cast<core::MouseButtonEvent &>(e);
			io.AddMouseButtonEvent(toImGuiButton(btn.getButton()), btn.isPressed());
			if (btn.isPressed() && ImGui::GetIO().WantCaptureMouse)
				e.handled = true;
		} else if (e.getTypeID() == core::TextInputEvent::staticTypeID()) {
			auto &text = static_cast<core::TextInputEvent &>(e);
			io.AddInputCharacter(text.getCodepoint());
		} else if (e.getTypeID() == core::WindowResizeEvent::staticTypeID()) {
			auto &size = static_cast<core::WindowResizeEvent &>(e);
			io.DisplaySize = ImVec2(size.getWidth(), size.getHeight());
		}
	}

	void ImGuiLayer::onRender(float t) {
		beginFrame();
		m_uiManager.render();
		endFrame();
	}

	void ImGuiLayer::resize(int width, int height) {
		ImGuiIO &io = ImGui::GetIO();
		io.DisplaySize = ImVec2(width, height);
	}

	void ImGuiLayer::stylesApply() {
		ImGuiStyle &style = ImGui::GetStyle();
		style.WindowRounding = 8.0f;
		style.FrameRounding = 4.0f;
		style.GrabRounding = 4.0f;
		style.PopupRounding = 4.0f;
		style.ScrollbarRounding = 4.0f;
		style.TabRounding = 4.0f;
	}

	void ImGuiLayer::configureApply() {
		ImGuiIO &io = ImGui::GetIO();
		io.IniFilename = nullptr;
	}
} // namespace triple::editor