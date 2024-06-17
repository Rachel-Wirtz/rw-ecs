#ifndef RW__ECS_ENTITY_POOL__H
#define RW__ECS_ENTITY_POOL__H

#include "rw-ecs.h"

RW_ECS_NAMESPACE_BEGIN

template<std::unsigned_integral Entity>
class basic_entity_pool {
public:
    using data_type      = std::vector<Entity>;
    using indices_type   = std::unordered_map<Entity, size_t>;
    using iterator       = typename data_type::iterator;
    using const_iterator = typename data_type::const_iterator;

    void push(Entity entity);
    void pop(Entity entity);

    bool contains(Entity entity) const noexcept;

    Entity count(void) const noexcept;

    iterator begin(void) noexcept;
    iterator end(void) noexcept;

    const_iterator begin(void) const noexcept;
    const_iterator end(void) const noexcept;

private:
    indices_type m_Indices{};
    data_type    m_Data{};
};

template<std::unsigned_integral Entity>
void basic_entity_pool<Entity>::push(Entity entity) {
    if (!m_Indices.contains(entity)) {
        m_Indices[entity] = m_Data.size();
        m_Data.push_back(entity);
    }
}

template<std::unsigned_integral Entity>
void basic_entity_pool<Entity>::pop(Entity entity) {
    auto it = m_Indices.find(entity);
    if (it == m_Indices.end()) return;

    size_t entity_index = it->second;
    size_t swap_index = m_Data.size() - 1;

    Entity& swap_entity = m_Data[swap_index];
    m_Indices[swap_entity] = entity_index;
    std::swap(swap_entity, m_Data[entity_index]);

    m_Indices.erase(entity);
    m_Data.pop_back();
}

template<std::unsigned_integral Entity>
bool basic_entity_pool<Entity>::contains(Entity entity) const noexcept {
    return m_Indices.contains(entity);
}

template<std::unsigned_integral Entity>
Entity basic_entity_pool<Entity>::count(void) const noexcept {
    size_t result = m_Data.size();
    assert(result < static_cast<size_t>(invalid_entity<Entity>));
    return result < invalid_entity<Entity> ? static_cast<Entity>(result) : invalid_entity<Entity>;
}

template<std::unsigned_integral Entity>
basic_entity_pool<Entity>::iterator basic_entity_pool<Entity>::begin(void) noexcept {
    return m_Data.begin();
}

template<std::unsigned_integral Entity>
basic_entity_pool<Entity>::iterator basic_entity_pool<Entity>::end(void) noexcept {
    return m_Data.end();
}

template<std::unsigned_integral Entity>
basic_entity_pool<Entity>::const_iterator basic_entity_pool<Entity>::begin(void) const noexcept {
    return m_Data.begin();
}

template<std::unsigned_integral Entity>
basic_entity_pool<Entity>::const_iterator basic_entity_pool<Entity>::end(void) const noexcept {
    return m_Data.end();
}

RW_ECS_NAMESPACE_END
#endif