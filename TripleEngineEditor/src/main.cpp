#include <iostream>
#include <Application.h>
#include <TripleEngineCore.h>

class EditorApp : public TripleEngineCore::Application {
	int index = 0;
	virtual void onUpdate() override {

	}
};

int main() {
	std::unique_ptr<EditorApp> app = std::make_unique<EditorApp>();
	app->start("Triple Editor v0.0.1", 500, 300);

	std::getchar();
	return 0;
}