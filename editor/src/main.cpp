#include <iostream>
#include "triple/editor/EditorApp.h"

using namespace triple::editor;
using namespace triple::core;

int main() {
	std::unique_ptr<EditorApp> app = std::make_unique<EditorApp>();
	app->init();
	app->loadCallbacks();

	Engine::ErrorCode code = app->run("Triple Editor v(0.0.1)", 800, 600);

	std::cout << "Press Enter to exit...";
	int key = std::getchar();
	return static_cast<int>(code);
}