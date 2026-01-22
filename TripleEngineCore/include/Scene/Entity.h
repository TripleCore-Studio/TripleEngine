#ifndef ENTITY_H
#define ENTITY_H

#include "Core/CoreTypes.h"

namespace TripleEngineCore::Scene {
	using Entity = Index;
	inline constexpr Entity INVALID_ENTITY = (std::numeric_limits<Entity>::max)();
}

#endif // ENTITY_H
