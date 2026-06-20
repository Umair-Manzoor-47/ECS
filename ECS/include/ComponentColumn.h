#pragma once
#include <algorithm>
#include <cstddef>
#include <new>
#include <cassert>

namespace ecs {
    class ComponentColumn {
    public:

        // Default Contructor
        ComponentColumn() = default;

        /// Forbidding copying
        ComponentColumn(const ComponentColumn&) = delete;
        ComponentColumn& operator=(const ComponentColumn&) = delete;

        /// Allowing moving
        /// SELF NOTE:
        /// m_destroy / m_moveConstruct / m_elementSize don't strictly need resetting,
        /// but m_data = nullptr is what matters most
        ComponentColumn(ComponentColumn&& other) noexcept {
            m_data = other.m_data;
            m_elementSize = other.m_elementSize;
            m_count = other.m_count;
            m_capacity = other.m_capacity;
            m_destroy = other.m_destroy;
            m_moveConstruct = other.m_moveConstruct;

            other.m_data = nullptr;
            other.m_count = 0;
            other.m_capacity = 0;
        }
        ComponentColumn& operator=(ComponentColumn&& other) noexcept {
            if (this == &other) {
                return *this;
            }

            this->~ComponentColumn();

            m_data = other.m_data;
            m_elementSize = other.m_elementSize;
            m_count = other.m_count;
            m_capacity = other.m_capacity;
            m_destroy = other.m_destroy;
            m_moveConstruct = other.m_moveConstruct;

            other.m_data = nullptr;
            other.m_count = 0;
            other.m_capacity = 0;

            return *this;
        }
        
        // Destructor
        ~ComponentColumn() {
            for (size_t i = 0; i < m_count; ++i) {
                void* slot = m_data + (i * m_elementSize);
                m_destroy(slot);
            }
            ::operator delete(m_data);
        }

        template<typename T>
        void PushBack(T value) {
            if (m_count == m_capacity) {
                grow();
            }
            void* slot = m_data + (m_count * m_elementSize);
            new (slot) T(std::move(value));
            ++m_count;
        }

        template<typename T>
        static ComponentColumn Create() {
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

        void* GetRow(const size_t row) const {
            assert(row < m_count && "GetRow: row out of bounds");
            return m_data + (row * m_elementSize);
        }

        void SwapAndPop(const size_t row) {
            assert(row < m_count && "SwapAndPop: row out of bounds");

            void* dst = m_data + (row * m_elementSize);
            void* src = m_data + ((m_count - 1) * m_elementSize);

            if (row == m_count - 1) {
                m_destroy(dst);
                --m_count;
                return;
            }

            m_destroy(dst);
            m_moveConstruct(dst, src);
            m_destroy(src);
            --m_count;
        }


    private:
        std::byte* m_data = nullptr;
        size_t m_elementSize = 0;
        size_t m_count = 0;
        size_t m_capacity = 0;

        void (*m_destroy)(void*) = nullptr;
        void (*m_moveConstruct)(void*, void*) = nullptr;

        void grow() {
            const size_t newCapacity = (m_capacity == 0) ? 4 : m_capacity * 2;
            std::byte* newData = static_cast<std::byte*>(::operator new(newCapacity * m_elementSize));

            for (size_t i = 0; i < m_count; ++i) {
                void* oldSlot = m_data + (i * m_elementSize);
                void* newSlot = newData + (i * m_elementSize);

                m_moveConstruct(newSlot, oldSlot);
                m_destroy(oldSlot);
            }

            ::operator delete(m_data);
            m_data = newData;
            m_capacity = newCapacity;
        }
    };
} // ecs