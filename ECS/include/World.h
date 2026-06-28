#pragma once
#include <ArchetypeRegistry.h>
#include <EntityRegistry.h>
#include <Entity.h>
#include <ComponentID.h>

namespace ecs {
    class World {

    public:
        Entity CreateEntity() {
            uint32_t id = m_nextEntityID++;
            return Entity(id);
        }
        void DestroyEntity(Entity entity) {
            auto& record = m_entityRegistry.Get(entity.GetID());
            Archetype* archetype = record.archetype;
            size_t row = record.row;
            size_t lastRow = archetype->GetRowCount() - 1;

            // find who was at the last row BEFORE the swap happens
            uint32_t swappedEntityID = archetype->GetEntityID(lastRow);

            archetype->RemoveRow(row);
            m_entityRegistry.Remove(entity.GetID());

            // fix the swapped entity's record — it moved from lastRow to row
            if (row != lastRow) {
                m_entityRegistry.Get(swappedEntityID).row = row;
            }
        }

        /// Step 1: build signature from the types
        /// Step 2: sort it
        /// Step 3: get or create the archetype
        /// Step 3b: if brand new, initialize its columns
        /// Step 4: push the actual component data
        /// Step 5: register the entity's location
        template<typename... Components>
        void AddComponents(const Entity entity, Components&&... components) {

            Signature sig = { ComponentID::get<Components>()... };
            std::sort(sig.begin(), sig.end());
            Archetype& archetype = m_archetypeRegistry.GetOrCreate(sig);
            if (archetype.IsEmpty()) {
                (archetype.AddColumn<Components>(), ...);
            }
            archetype.PushBack(entity.GetID(), std::forward<Components>(components)...);
            m_entityRegistry.Add(entity.GetID(), &archetype, archetype.GetRowCount() - 1);
        }

        template<typename Component>
        Component& GetComponent(Entity entity) {
            auto& record = m_entityRegistry.Get(entity.GetID());
            assert(record.archetype->HasComponent(ComponentID::get<Component>())
                && "Entity does not have this component");
            return record.archetype->GetComponent<Component>(record.row);
        }

        /// 1. m_archetypeRegistry.GetArchetypes() — iterate all archetypes
        /// 2. std::includes — check if archetype matches query
        /// 3. archetype.GetRowCount() — how many rows to iterate
        /// 4. archetype.GetComponent<T>(row) — get component reference per row

        template<typename... Components, typename Func>
        void Query(Func&& func) {
            Signature querySig = { ComponentID::get<Components>()... };
            std::ranges::sort(querySig);

            for (auto& [sig, arch] : m_archetypeRegistry.GetArchetypes()) {
                if (!std::ranges::includes(sig, querySig)) {
                    continue;
                                   }
                for (size_t row = 0; row < arch.GetRowCount(); ++row) {
                    func(arch.GetComponent<Components>(row)...);
                }
            }
        }
        size_t GetArchetypeCount() const {
            return m_archetypeRegistry.GetArchetypeCount();
        }
    private:
        ArchetypeRegistry m_archetypeRegistry;
        EntityRegistry m_entityRegistry;
        uint32_t m_nextEntityID = 0;
    };
} // ecs
