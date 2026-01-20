#include <iostream>
#include "EditorApp.h"

using namespace TripleEngineEditor;
using namespace TripleEngineCore;

int main() {
	std::unique_ptr<EditorApp> app = std::make_unique<EditorApp>();
	app->loadCallbacks();

	Engine::ErrorCode code = app->start("Triple Editor v(0.0.1)", 800, 600);
	
	std::cout << "Press Enter to exit...";
	int key = std::getchar();
	return static_cast<int>(code);
}