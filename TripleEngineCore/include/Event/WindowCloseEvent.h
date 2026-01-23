#ifndef WINDOW_CLOSE_EVENT_H
#define WINDOW_CLOSE_EVENT_H

#include "Event/Event.h"

namespace TripleEngineCore::Event {
	class CORE_API WindowCloseEvent : public Event {
	public:
		WindowCloseEvent(const char* title)
			: _title(const_cast<char*>(title)) {
		}

		EventID getTypeID() const override {
			return typeid(WindowCloseEvent);
		}

		const char* getTitle() const { return _title; }
	private:
		char* _title;
	};
}

#endif // WINDOW_CLOSE_EVENT_H
