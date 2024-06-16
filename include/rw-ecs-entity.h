#ifndef RW__ECS_ENTITY__H
#define RW__ECS_ENTITY__H

#include "rw-ecs.h"

RW_ECS_NAMESPACE_BEGIN

using entity_handle = uint64_t;

class entity_pool {
public:
    using iterator       = std::vector<entity_handle>::iterator;
    using const_iterator = std::vector<entity_handle>::const_iterator;

    void add_entity(entity_handle entity) {
        if (!m_Indices.contains(entity)) {
            m_Indices[entity] = m_Entities.size();
            m_Entities.push_back(entity);
        }
    }

    void remove_entity(entity_handle entity) {
        auto it = m_Indices.find(entity);
        if (it == m_Indices.end()) return;

        size_t entity_index = it->second;
        size_t swap_index = m_Entities.size() - 1;

        if (entity_index == swap_index) {
            m_Indices.erase(entity);
            m_Entities.pop_back();
            return;
        }

        entity_handle& swap_entity = m_Entities[swap_index];
        m_Indices[swap_entity] = entity_index;
        std::swap(swap_entity, m_Entities[entity_index]);

        m_Indices.erase(entity);
        m_Entities.pop_back();
    }

    bool contains_entity(entity_handle entity) const noexcept {
        return m_Indices.contains(entity);
    }

    iterator begin(void) noexcept {
        return m_Entities.begin();
    }

    iterator end(void) noexcept {
        return m_Entities.end();
    }

    const_iterator begin(void) const noexcept {
        return m_Entities.begin();
    }

    const_iterator end(void) const noexcept {
        return m_Entities.end();
    }

private:
    std::unordered_map<entity_handle, size_t> m_Indices{};
    std::vector<entity_handle>                m_Entities{};
};

RW_ECS_NAMESPACE_END

#endif