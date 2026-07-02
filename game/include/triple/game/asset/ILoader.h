#pragma once

#include <optional>

#include "ILoaderBase.h"
#include "LoadersParams.h"

namespace triple::game {
	template <typename T>
	struct ILoader : public ILoaderBase {
		virtual std::optional<T> load(const typename LoadParamsOf<T>::Type &params) = 0;
		virtual ~ILoader() = default;
	};
} // namespace triple::game