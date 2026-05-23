#include "triple/core/ecs/ComponentService.h"
#include <stdexcept>

namespace triple::core {
	ComponentTypeID ComponentService::registerComponent(size_t size, size_t align,
	                                                    void (*construct)(void *),
	                                                    void (*destruct)(void *),
	                                                    void (*move)(void *, void *)) {
		if (!construct || !destruct || !move || size == 0) {
			throw std::invalid_argument("Invalid component registration");
		}

		ComponentTypeID id = m_nextID++;
		m_registry[id] = {size, align, construct, destruct, move};
		return id;
	}

	const ComponentInfo &ComponentService::getInfo(ComponentTypeID id) const {
		auto it = m_registry.find(id);
		if (it == m_registry.end()) {
			throw std::runtime_error("ComponentTypeID not registered");
		}
		return it->second;
	}
	ComponentTypeID ComponentService::getTypeByIndex(const std::type_index &index) const {
		auto it = m_typeMap.find(index);
		if (it == m_typeMap.end()) {
			return INVALID_COMPONENT_TYPE_ID;
		}
		return it->second;
	}
} // namespace triple::core