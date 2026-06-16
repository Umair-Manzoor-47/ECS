#pragma once
#include <algorithm>
#include <cstddef>
#include <new>
namespace ecs {
    class ComponentColumn {

    private:
        std::byte* m_data = nullptr;
        size_t m_elementSize = 0;
        size_t m_count = 0;
        size_t m_capacity = 0;

        void (*m_destroy)(void*) = nullptr;
        void (*m_moveConstruct)(void*, void*) = nullptr;
        template<typename T>
        static ComponentColumn create() {
                    ComponentColumn col;
                    col.m_elementSize = sizeof(T);

                    col.m_destroy = [](void* obj) {
                        static_cast<T*>(obj)->~T();
                    };

                    col.m_moveConstruct = [](void* dst, void* src) {
                        new (dst) T(std::move(*static_cast<T*>(src)));
                    };

                    return col;
        }
    };
} // ecs