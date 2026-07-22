#include "triple/game/asset/TextureLoader.h"

#include <stb_image.h>

#include <triple/log/Logger.h>

namespace triple::game {
	std::optional<Texture> TextureLoader::load(const TextureLoadParams &params) {
		stbi_set_flip_vertically_on_load(true);

		int width, height, channels;
		stbi_uc *pixels = stbi_load(params.path.c_str(), &width, &height, &channels, 4);
		if (!pixels) {
			triple::log::Logger::moduleError("TextureLoader", "Failed to load texture '{}'",
			                                 params.path);
			return std::nullopt;
		}

		Texture texture;
		texture.width = static_cast<uint16_t>(width);
		texture.height = static_cast<uint16_t>(height);
		texture.channels = 4;
		size_t pixelCount =
		    static_cast<size_t>(width) * static_cast<size_t>(height) * texture.channels;
		texture.pixels.assign(pixels, pixels + pixelCount);

		stbi_image_free(pixels);

		return std::move(texture);
	}
} // namespace triple::game