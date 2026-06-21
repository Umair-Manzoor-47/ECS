#pragma once
#include <cstdint>

class Entity {
public:
    explicit Entity(uint32_t id) : m_entityID(id) {}

    uint32_t GetID() const { return m_entityID; }

private:
    uint32_t m_entityID;
};