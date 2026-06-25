#include "triple/editor/UI/ImGuiLayer.h"

#include <triple/core/event/KeyboardInputEvent.h>
#include <triple/core/event/MouseMoveEvent.h>
#include <triple/core/event/MouseButtonEvent.h>
#include <triple/core/event/WindowResizeEvent.h>
#include <triple/core/event/TextInputEvent.h>

#include <triple/core/input/KeyCode.h>
#include <triple/core/input/MouseButton.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <ImGuiFileDialog.h>

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
		} else if (e.getTypeID() == core::TextInputEvent::staticTypeID()) {
			auto &text = static_cast<core::TextInputEvent &>(e);
			io.AddInputCharacter(text.getCodepoint());
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