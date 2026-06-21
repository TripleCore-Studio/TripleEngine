#include "triple/editor/UI/ImGuiLayer.h"

#include <triple/core/event/KeyboardInputEvent.h>
#include <triple/core/event/MouseMoveEvent.h>
#include <triple/core/event/MouseButtonEvent.h>
#include <triple/core/event/WindowResizeEvent.h>

#include <triple/core/input/KeyCode.h>
#include <triple/core/input/MouseButton.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <ImGuiFileDialog.h>

namespace triple::editor {
	ImGuiKey toImGuiKey(core::KeyCode key) {
		using namespace triple::core;

		switch (key) {
		case KeyCode::A:
			return ImGuiKey_A;
		case KeyCode::B:
			return ImGuiKey_B;
		case KeyCode::C:
			return ImGuiKey_C;
		case KeyCode::D:
			return ImGuiKey_D;
		case KeyCode::W:
			return ImGuiKey_W;
		case KeyCode::S:
			return ImGuiKey_S;

		case KeyCode::F1:
			return ImGuiKey_F1;
		case KeyCode::F2:
			return ImGuiKey_F2;
		case KeyCode::F3:
			return ImGuiKey_F3;
		case KeyCode::F4:
			return ImGuiKey_F4;
		case KeyCode::F5:
			return ImGuiKey_F5;
		case KeyCode::F6:
			return ImGuiKey_F6;
		case KeyCode::F7:
			return ImGuiKey_F7;
		case KeyCode::F8:
			return ImGuiKey_F8;
		case KeyCode::F9:
			return ImGuiKey_F9;
		case KeyCode::F10:
			return ImGuiKey_F10;
		case KeyCode::F11:
			return ImGuiKey_F11;
		case KeyCode::F12:
			return ImGuiKey_F12;

		case KeyCode::Space:
			return ImGuiKey_Space;
		case KeyCode::Escape:
			return ImGuiKey_Escape;

		case KeyCode::LeftShift:
			return ImGuiKey_LeftShift;
		case KeyCode::RightShift:
			return ImGuiKey_RightShift;
		case KeyCode::LeftCtrl:
			return ImGuiKey_LeftCtrl;
		case KeyCode::RightCtrl:
			return ImGuiKey_RightCtrl;

		case KeyCode::Up:
			return ImGuiKey_UpArrow;
		case KeyCode::Down:
			return ImGuiKey_DownArrow;
		case KeyCode::Left:
			return ImGuiKey_LeftArrow;
		case KeyCode::Right:
			return ImGuiKey_RightArrow;

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

	ImGuiLayer::~ImGuiLayer() { shutdown(); }

	void ImGuiLayer::init() {
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		if (!ImGui_ImplOpenGL3_Init("#version 330")) {
			m_initialized = false;
			return;
		}
		configureApply();
		stylesApply();
		m_initialized = true;
	}

	void ImGuiLayer::beginFrame(float dt) {
		ImGuiIO &io = ImGui::GetIO();
		io.DeltaTime = dt;
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::endFrame() {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiLayer::shutdown() const {
		if (m_initialized) {
			ImGui_ImplOpenGL3_Shutdown();
			ImGui::DestroyContext();
		}
	}

	void ImGuiLayer::onEvent(core::Event &e) const {
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
		} else if (e.getTypeID() == core::WindowResizeEvent::staticTypeID()) {
			auto &size = static_cast<core::WindowResizeEvent &>(e);
			io.DisplaySize = ImVec2(size.getWidth(), size.getHeight());
		}
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