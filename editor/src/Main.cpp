#include "triple/editor/Editor.h"

#include <memory>

using namespace triple::editor;
using namespace triple::core;

int main() {
	std::unique_ptr<Editor> app = std::make_unique<Editor>();
	app->init();
	app->loadCallbacks();

	Engine::ErrorCode code = app->run("Triple Engine v(0.1.0-pre-alpha)", 1280, 720);

	return static_cast<int>(code);
}