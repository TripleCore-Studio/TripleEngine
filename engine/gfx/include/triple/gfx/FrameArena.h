#pragma once

#include <cassert>
#include <cstdint>
#include <vector>

namespace triple::gfx {
	class FrameArena {
	public:
		void init(size_t capacity) {
			m_buffer.resize(capacity);
			m_offset = 0;
		}

		[[nodiscard]] uint8_t *allocate(size_t size) {
			assert(m_offset + size <= m_buffer.size() && "FrameArena overflow, increase capacity");
			uint8_t *ptr = m_buffer.data() + m_offset;
			m_offset += size;
			return ptr;
		}

		void reset() { m_offset = 0; }

		[[nodiscard]] size_t used() const { return m_offset; }

		[[nodiscard]] size_t capacity() const { return m_buffer.size(); }

	private:
		std::vector<uint8_t> m_buffer;
		size_t m_offset = 0;
	};
} // namespace triple::gfx