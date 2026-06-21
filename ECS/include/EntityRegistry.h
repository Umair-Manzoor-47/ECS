#pragma once
#include <Archetype.h>
#include <unordered_map>

namespace ecs {
    struct EntityRecord {
        Archetype* archetype = nullptr;
        size_t row = 0;
    };
    class EntityRegistry {
    public:
        void Add(uint32_t entityID, Archetype* archetype, const size_t row) {
            m_entities.emplace(entityID, EntityRecord{archetype, row});
        }
        EntityRecord& Get(const uint32_t entityID) {
            auto it = m_entities.find(entityID);
            assert(it != m_entities.end() && "Entity not found");
            return it->second;
        }
        bool Remove(const uint32_t entityID) {
            const auto it = m_entities.find(entityID);
            if (it == m_entities.end()) return false;
            m_entities.erase(it);
            return true;
        }

    private:
        std::unordered_map<uint32_t, EntityRecord> m_entities;
    };
}