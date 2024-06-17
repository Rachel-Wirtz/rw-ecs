#include "rw-ecs.h"
RW_ECS_NAMESPACE_BEGIN

void entity_pool::push(entity_handle entity) {
    if (!m_Indices.contains(entity)) {
        m_Indices[entity] = m_Data.size();
        m_Data.push_back(entity);
    }
}

void entity_pool::pop(entity_handle entity) {
    auto it = m_Indices.find(entity);
    if (it == m_Indices.end()) return;

    size_t entity_index = it->second;
    size_t swap_index = m_Data.size() - 1;

    entity_handle& swap_entity = m_Data[swap_index];
    m_Indices[swap_entity] = entity_index;
    std::swap(swap_entity, m_Data[entity_index]);

    m_Indices.erase(entity);
    m_Data.pop_back();
}

bool entity_pool::contains(entity_handle entity) const noexcept {
    return m_Indices.contains(entity);
}

entity_handle entity_pool::count(void) const noexcept {
    size_t result = m_Data.size();
    assert(result < static_cast<size_t>(invalid_entity));
    return result < invalid_entity ? static_cast<entity_handle>(result) : invalid_entity;
}

entity_pool::iterator entity_pool::begin(void) noexcept {
    return m_Data.begin();
}

entity_pool::iterator entity_pool::end(void) noexcept {
    return m_Data.end();
}

entity_pool::const_iterator entity_pool::begin(void) const noexcept {
    return m_Data.begin();
}

entity_pool::const_iterator entity_pool::end(void) const noexcept {
    return m_Data.end();
}

RW_ECS_NAMESPACE_END