#pragma once
#include <unordered_map>
#include <vector>
#include <cstdint>
#include "ComponentColumn.h"

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

    private:
        Signature m_signature;
        /// [ComponentColumn] is mapped against [ComponentID]
        std::unordered_map<uint32_t, ComponentColumn> m_columns;
    };
} // ecs