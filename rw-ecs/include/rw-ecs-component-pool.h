#ifndef RW__ECS_COMPONENT_POOL__H
#define RW__ECS_COMPONENT_POOL__H
RW_ECS_NAMESPACE_BEGIN

class icomponent_pool {
public:
    virtual ~icomponent_pool() = default;
    virtual void destroy_entity(entity_handle entity) = 0;
};

template<typename Component>
class component_pool : public icomponent_pool {
public:
    using data_type      = std::vector<Component>;
    using indices_type   = std::unordered_map<entity_handle, size_t>;
    using entities_type  = std::unordered_map<size_t, entity_handle>;
    using iterator       = typename data_type::iterator;
    using const_iterator = typename data_type::const_iterator;

    template<typename ... Args> requires std::constructible_from<Component, Args...>
    Component& push(entity_handle entity, Args&& ... args);

    void pop(entity_handle entity);

    Component& get(entity_handle entity);

    bool contains(entity_handle entity) const noexcept;

private:
    void destroy_entity(entity_handle entity) override;

private:
    indices_type  m_Indices{};
    entities_type m_Entities{};
    data_type     m_Components{};
};

template<typename Component>
template<typename ... Args> requires std::constructible_from<Component, Args...>
Component& component_pool<Component>::push(entity_handle entity, Args&& ... args) {
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

template<typename Component>
void component_pool<Component>::pop(entity_handle entity) {
    auto it = m_Indices.find(entity);
    if (it == m_Indices.end()) return;

    size_t entity_index = it->second;
    size_t swap_index = m_Components.size() - 1;

    entity_handle swap_entity = m_Entities[swap_index];
    m_Indices[swap_entity] = entity_index;
    m_Entities[entity_index] = swap_entity;
    std::swap(m_Components[entity_index], m_Components[swap_index]);

    m_Indices.erase(entity);
    m_Entities.erase(entity_index);
    m_Components.pop_back();
}

template<typename Component>
Component& component_pool<Component>::get(entity_handle entity) {
    return m_Components.at(m_Indices.at(entity));
}

template<typename Component>
bool component_pool<Component>::contains(entity_handle entity) const noexcept {
    return m_Indices.contains(entity);
}

template<typename Component>
void component_pool<Component>::destroy_entity(entity_handle entity) {
    this->pop(entity);
}

RW_ECS_NAMESPACE_END
#endif