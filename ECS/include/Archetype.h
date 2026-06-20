#pragma once
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <ComponentColumn.h>
#include <ComponentID.h>
#include <cassert>
namespace ecs {
    /* NOTE:
        Signature is important to keep and identify the archetype
        EXAMPLE: Archetype A -> [Comp0, Comp1]
                 Archetype B -> [Comp2, Comp1]
    */
    using Signature = std::vector<uint32_t>;


    class Archetype {
    public:
        explicit Archetype(Signature signature) : m_signature(std::move(signature)) {}
        const Signature& GetSignature() const { return m_signature; }

        bool HasComponent(const uint32_t componentID) const {
            return m_columns.contains(componentID);
        }

        template<typename Component>
        void AddColumn() {
            uint32_t id = ComponentID::get<Component>();
            m_columns.emplace(id, ComponentColumn::Create<Component>());
        }

        template<typename... Components>
        void PushBack(Components&&... components) {
            assert(sizeof...(Components) == m_columns.size()
                && "PushBack component count must match archetype column count");

            (m_columns.at(ComponentID::get<Components>())
                .PushBack(std::forward<Components>(components)), ...);
            ++m_rowCount;
        }

        void RemoveRow(size_t row) {
            assert(row < GetRowCount() && "RemoveRow: row out of bounds");
            for (auto& [id, column] : m_columns) {
                column.SwapAndPop(row);
            }
            --m_rowCount;
        }
        size_t GetRowCount() const { return m_rowCount; }

        template<typename Component>
        Component& GetComponent(size_t row) {
            uint32_t id = ComponentID::get<Component>();
            assert(m_columns.contains(id) && "Archetype does not have this component");
            void* ptr = m_columns.at(id).GetRow(row);
            return *static_cast<Component*>(ptr);
        }

    private:
        Signature m_signature;
        size_t m_rowCount = 0;
        /// [ComponentColumn] is mapped against [ComponentID]
        std::unordered_map<uint32_t, ComponentColumn> m_columns;
    };
} // ecs