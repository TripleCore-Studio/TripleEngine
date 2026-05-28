#include "triple/editor/UI/ImGuiLayer.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuiFileDialog.h>

namespace triple::editor {
	ImGuiLayer::~ImGuiLayer() { shutdown(); }

	void ImGuiLayer::init(core::IWindow *window) {
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		if (!ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *)window->GetNativeWindow(), true) ||
		    !ImGui_ImplOpenGL3_Init("#version 330")) {
			m_initialized = false;
			return;
		}
		configureApply();
		stylesApply();
		m_initialized = true;
	}

	void ImGuiLayer::beginFrame() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::endFrame() {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiLayer::shutdown() const {
		if (m_initialized) {
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}
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