#ifndef ASSET_STORAGE_H
#define ASSET_STORAGE_H

#include <unordered_map>
#include <string>
#include <vector>

#include "IAssetStorage.h"
#include "AssetTypes.h"

namespace triple::game {
	template <typename T>
	struct Slot {
		T value;
		std::string name;
		uint32_t generation;
		bool occupied;
	};

	template <typename T>
	struct InsertResult {
		TypedAssetID<T> id;
		bool wasReload;
	};

	template <typename T>
	class AssetStorage : public IAssetStorage {
	public:
		InsertResult<T> insert(std::string name, T asset) {
			InsertResult<T> result;

			if (auto it = m_nameToSlot.find(name); it != m_nameToSlot.end()) {
				Slot<T> &slot = m_slots[it->second];
				slot.value = std::move(asset);

				result.wasReload = true;
				result.id = TypedAssetID<T>{{it->second, slot.generation}};

				return result;
			}

			if (!m_freeSlots.empty()) {
				uint32_t freeSlot = m_freeSlots.back();
				m_freeSlots.pop_back();
				m_nameToSlot[name] = freeSlot;

				Slot<T> &slot = m_slots[freeSlot];
				slot.generation++;
				slot.occupied = true;
				slot.value = std::move(asset);
				slot.name = std::move(name);

				result.id = TypedAssetID<T>{{freeSlot, slot.generation}};
				result.wasReload = false;

				return result;
			} else {
				uint32_t newSlot = static_cast<uint32_t>(m_slots.size());
				m_nameToSlot[name] = newSlot;

				Slot<T> slot{std::move(asset), std::move(name), 0, true};
				m_slots.push_back(std::move(slot));

				result.id = TypedAssetID<T>{{newSlot, 0}};
				result.wasReload = false;

				return result;
			}
		}
		[[nodiscard]] const T *get(TypedAssetID<T> id) const {
			if (id.raw.slot >= m_slots.size())
				return nullptr;

			const Slot<T> &slot = m_slots[id.raw.slot];
			if (!slot.occupied || slot.generation != id.raw.generation)
				return nullptr;

			return &slot.value;
		}
		T *getMutable(TypedAssetID<T> id) {
			if (id.raw.slot >= m_slots.size())
				return nullptr;

			Slot<T> &slot = m_slots[id.raw.slot];
			if (!slot.occupied || slot.generation != id.raw.generation)
				return nullptr;

			return &slot.value;
		}
		[[nodiscard]] TypedAssetID<T> findByName(const std::string &name) const {
			auto it = m_nameToSlot.find(name);
			if (it == m_nameToSlot.end())
				return TypedAssetID<T>{};

			uint32_t slot = it->second;
			return TypedAssetID<T>{{slot, m_slots[slot].generation}};
		}

		[[nodiscard]] bool contains(AssetID id) const override {
			if (id.slot >= m_slots.size())
				return false;
			const Slot<T> &slot = m_slots[id.slot];
			return slot.occupied && slot.generation == id.generation;
		}
		bool remove(AssetID id) override {
			if (id.slot >= m_slots.size())
				return false;
			Slot<T> &slot = m_slots[id.slot];
			if (!slot.occupied || slot.generation != id.generation)
				return false;

			slot.occupied = false;
			m_freeSlots.push_back(id.slot);
			m_nameToSlot.erase(slot.name);
			return true;
		}

	private:
		std::vector<Slot<T>> m_slots;
		std::vector<uint32_t> m_freeSlots;
		std::unordered_map<std::string, uint32_t> m_nameToSlot;
	};
} // namespace triple::game

#endif // ASSET_STORAGE_H
