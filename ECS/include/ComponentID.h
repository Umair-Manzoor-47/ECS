#pragma once
#include <cstdint>

namespace ecs {
    class ComponentID {
    public:
        template<typename T>
        static uint32_t get() {
            static uint32_t typeID = m_counter++;
            return  typeID;
        };

    private:
        static uint32_t m_counter;
    };
} // ecs