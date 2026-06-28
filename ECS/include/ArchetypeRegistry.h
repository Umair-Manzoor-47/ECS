#pragma once
#include <unordered_map>
#include <Archetype.h>
namespace ecs {
    struct SignatureHasher {
        size_t operator()(const Signature& sig) const {
            size_t seed = 0;
            for (uint32_t id : sig) {
                // hash combine — standard trick borrowed from boost::hash_combine
                seed ^= std::hash<uint32_t>{}(id) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
    class ArchetypeRegistry {
    public:
        Archetype& GetOrCreate(const Signature& sig) {
            auto it = m_archetypes.find(sig);
            if (it != m_archetypes.end()) {
                return it->second;
            }
            auto [newIt, success] = m_archetypes.emplace(sig, Archetype(sig));
            return newIt->second;
        }
        const std::unordered_map<Signature, Archetype, SignatureHasher>& GetArchetypes() const { return m_archetypes; }
        size_t GetArchetypeCount() const {
            return m_archetypes.size();
        }
    private:
        std::unordered_map<Signature, Archetype, SignatureHasher> m_archetypes;

    };
}
