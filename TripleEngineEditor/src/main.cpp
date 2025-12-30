#include <iostream>
#include <Engine/Engine.h>

using namespace TripleEngineCore;
using namespace TripleMath;


class EditorApp : public TripleEngineCore::Engine {
	int index = 0;
};

int main() {
	std::unique_ptr<EditorApp> app = std::make_unique<EditorApp>();
	return (int)app->start("Triple Editor v(0.0.1)", 800, 600);
}