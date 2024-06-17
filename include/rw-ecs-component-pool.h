#ifndef RW__ECS_COMPONENT_POOL__H
#define RW__ECS_COMPONENT_POOL__H

#include "rw-ecs.h"

RW_ECS_NAMESPACE_BEGIN

template<std::unsigned_integral Entity>
class basic_component_pool_base {
public:
    virtual ~basic_component_pool_base() = default;
    virtual void destroy_entity(Entity entity) = 0;
};

template<std::unsigned_integral Entity, typename Component>
class basic_component_pool : public basic_component_pool_base<Entity> {
public:
    using data_type      = std::vector<Component>;
    using indices_type   = std::unordered_map<Entity, size_t>;
    using entities_type  = std::unordered_map<size_t, Entity>;
    using iterator       = typename data_type::iterator;
    using const_iterator = typename data_type::const_iterator;

    template<typename ... Args> requires std::constructible_from<Component, Args...>
    Component& push(Entity entity, Args&& ... args);
    void       pop(Entity entity);
    Component& get(Entity entity);
    bool       contains(Entity entity) const noexcept;

private:
    void destroy_entity(Entity entity) override;

private:
    indices_type  m_Indices{};
    entities_type m_Entities{};
    data_type     m_Components{};
};

template<std::unsigned_integral Entity, typename Component>
template<typename ... Args> requires std::constructible_from<Component, Args...>
Component& basic_component_pool<Entity, Component>::push(Entity entity, Args&& ... args) {
    auto it = m_Indices.find(entity);

    if (it != m_Indices.end()) {
        Component& result = m_Components[it->second];
        result = Component(std::forward<Args>(args)...);
        return result;
    }

    size_t new_index = m_Components.size();
    m_Indices[entity] = new_index;
    m_Entities[new_index] = entity;

    return m_Components.emplace_back(std::forward<Args>(args)...);
}

template<std::unsigned_integral Entity, typename Component>
void basic_component_pool<Entity, Component>::pop(Entity entity) {
    auto it = m_Indices.find(entity);
    if (it == m_Indices.end()) return;

    size_t entity_index = it->second;
    size_t swap_index = m_Components.size() - 1;

    Entity swap_entity = m_Entities[swap_index];
    m_Indices[swap_entity] = entity_index;
    m_Entities[entity_index] = swap_entity;
    std::swap(m_Components[entity_index], m_Components[swap_index]);

    m_Indices.erase(entity);
    m_Entities.erase(entity_index);
    m_Components.pop_back();
}

template<std::unsigned_integral Entity, typename Component>
Component& basic_component_pool<Entity, Component>::get(Entity entity) {
    return m_Components.at(m_Indices.at(entity));
}

template<std::unsigned_integral Entity, typename Component>
bool basic_component_pool<Entity, Component>::contains(Entity entity) const noexcept {
    return m_Indices.contains(entity);
}

template<std::unsigned_integral Entity, typename Component>
void basic_component_pool<Entity, Component>::destroy_entity(Entity entity) {
    this->pop(entity);
}

RW_ECS_NAMESPACE_END
#endif