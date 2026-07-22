#pragma once

#include <vector>

#include <triple/gfx/GfxTypes.h>

#include "IResourcePool.h"

namespace triple::gl {
	template <typename T>
	class ResourcePool final : public IResourcePool {
	public:
		gfx::GpuHandle create(T &&value) {
			uint32_t idx;
			if (!m_freeList.empty()) {
				idx = m_freeList.back();
				m_freeList.pop_back();
			} else {
				idx = static_cast<uint32_t>(m_slots.size());
				m_slots.push_back({});
			}
			auto &slot = m_slots[idx];
			slot.data = std::move(value);
			slot.alive = true;
			return gfx::GpuHandle{idx, slot.generation};
		}

		T *get(gfx::GpuHandle h) {
			if (h == gfx::kInvalidGpuHandle || h.slot >= m_slots.size())
				return nullptr;
			auto &slot = m_slots[h.slot];
			return (slot.alive && slot.generation == h.generation) ? &slot.data : nullptr;
		}

		template <typename Fn>
		void forEachAlive(Fn &&fn) {
			for (auto &slot : m_slots)
				if (slot.alive)
					fn(slot.data);
		}

		bool destroy(gfx::GpuHandle h) override {
			T *ptr = get(h);
			if (!ptr)
				return false;
			auto &slot = m_slots[h.slot];
			slot.alive = false;
			slot.generation++;
			m_freeList.push_back(h.slot);
			return true;
		}

	private:
		struct Slot {
			T data{};
			uint32_t generation = 0;
			bool alive = false;
		};
		std::vector<Slot> m_slots;
		std::vector<uint32_t> m_freeList;
	};
} // namespace triple::gl