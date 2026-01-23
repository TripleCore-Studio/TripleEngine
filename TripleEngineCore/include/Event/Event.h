#ifndef EVENT_H
#define EVENT_H

#include <ExportMacros.h>
#include <typeindex>

namespace TripleEngineCore::Event {
	using EventID = std::type_index;

	class CORE_API Event {
	public:
		virtual ~Event() = default;
		virtual EventID getTypeID() const = 0;
	};
}

#endif // EVENT_H
