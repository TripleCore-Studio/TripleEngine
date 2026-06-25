#ifndef TEXT_INPUT_EVENT_H
#define TEXT_INPUT_EVENT_H

#include <triple/core/base/ExportMacros.h>

#include "EventBase.h"

namespace triple::core {
	class CORE_API TextInputEvent : public EventBase<TextInputEvent> {
	public:
		explicit TextInputEvent(unsigned int codepoint) : m_codepoint(codepoint) {}

		[[nodiscard]] unsigned int getCodepoint() const { return m_codepoint; }
		[[nodiscard]] char toChar() const {
			return m_codepoint < 128 ? static_cast<char>(m_codepoint) : '?';
		}

	private:
		unsigned int m_codepoint;
	};
} // namespace triple::core

#endif // TEXT_INPUT_EVENT_H
