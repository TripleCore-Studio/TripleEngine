#include <iostream>
#include <Application/Application.h>

using namespace TripleEngineCore;
using namespace TripleMath;


class EditorApp : public TripleEngineCore::Application {
	int index = 0;
};

int main() {
	std::unique_ptr<EditorApp> app = std::make_unique<EditorApp>();
	app->BootstrapResources();
	app->DemoScene();
	return (int)app->start("Triple Editor v(0.0.1)", 800, 600);
}