#ifndef EVENT_H
#define EVENT_H

#include <typeindex>
#include "triple/core/base/ExportMacros.h"

namespace triple::core {
	using EventID = std::type_index;

	class CORE_API Event {
	public:
		virtual ~Event() = default;
		[[nodiscard]] virtual EventID getTypeID() const = 0;

		bool handled = false;
	};
} // namespace triple::core

#endif // EVENT_H
