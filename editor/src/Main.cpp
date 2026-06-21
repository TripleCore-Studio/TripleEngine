#include <memory>
#include <triple/core/base/Engine.h>
#include <triple/log/Logger.h>

#include <triple/game/GameLayer.h>

#include "triple/editor/EditorLayer.h"

int main() {
	std::unique_ptr<triple::core::Engine> app = std::make_unique<triple::core::Engine>();
	app->init();

	auto game = std::make_unique<triple::game::GameLayer>();
	auto editor = std::make_unique<triple::editor::EditorLayer>(game.get());

	game->setFrameContextCallback([editorPtr = editor.get()](triple::gfx::FrameContext &ctx) {
		editorPtr->fillFrameContext(ctx);
	});

	app->pushLayer(std::move(game));
	app->pushLayer(std::move(editor));

	triple::core::Engine::ErrorCode code = app->run("Triple Engine v(0.1.0-pre-alpha)", 1280, 720);

	return static_cast<int>(code);
}