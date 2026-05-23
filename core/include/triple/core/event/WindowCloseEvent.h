#ifndef WINDOW_CLOSE_EVENT_H
#define WINDOW_CLOSE_EVENT_H

#include "Event.h"

namespace triple::core {
	class CORE_API WindowCloseEvent : public Event {
	  public:
		WindowCloseEvent(const char *title) : m_title(const_cast<char *>(title)) {}

		EventID getTypeID() const override { return typeid(WindowCloseEvent); }

		const char *getTitle() const { return m_title; }

	  private:
		char *m_title;
	};
} // namespace triple::core

#endif // WINDOW_CLOSE_EVENT_H
