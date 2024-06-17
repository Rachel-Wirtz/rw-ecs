#ifndef RW__ECS_COMPONENT_MANAGER__H
#define RW__ECS_COMPONENT_MANAGER__H
RW_ECS_NAMESPACE_BEGIN

class component_manager {
public:
    template<typename Component>
    void register_component(void);

    template<typename Component, typename ... Args> requires std::constructible_from<Component, Args...>
    Component& add_component(entity_handle entity, Args&& ... args);

    template<typename Component>
    void remove_component(entity_handle entity);

    template<typename Component>
    Component& get_component(entity_handle entity);

    template<typename Component>
    bool has_component(entity_handle entity) const noexcept;

private:
    template<typename Component>
    component_pool<Component>& get_pool(void);

    template<typename Component>
    const component_pool<Component>& get_pool(void) const;

    void destroy_entity(entity_handle entity);

private:
    std::unordered_map<std::type_index, std::unique_ptr<icomponent_pool>> m_Data{};

    friend class entity_component_system;
};

template<typename Component>
void component_manager::register_component(void) {
    if (m_Data.contains(typeid(Component))) return;
    m_Data[typeid(Component)] = std::make_unique<component_pool<Component>>();
}

template<typename Component, typename ... Args> requires std::constructible_from<Component, Args...>
Component& component_manager::add_component(entity_handle entity, Args&& ... args) {
    component_pool<Component>& pool = this->get_pool<Component>();
    return pool.push(entity, std::forward<Args>(args)...);
}

template<typename Component>
void component_manager::remove_component(entity_handle entity) {
    component_pool<Component>& pool = this->get_pool<Component>();
    pool.pop(entity);
}

template<typename Component>
Component& component_manager::get_component(entity_handle entity) {
    component_pool<Component>& pool = this->get_pool<Component>();
    return pool.get(entity);
}

template<typename Component>
bool component_manager::has_component(entity_handle entity) const noexcept {
    if (!m_Data.contains(typeid(Component))) return false;
    const component_pool<Component>& pool = this->get_pool<Component>();
    return pool.contains(entity);
}

template<typename Component>
component_pool<Component>& component_manager::get_pool(void) {
    std::unique_ptr<icomponent_pool>& base_pool = m_Data.at(typeid(Component));
    return *static_cast<component_pool<Component>*>(base_pool.get());
}

template<typename Component>
const component_pool<Component>& component_manager::get_pool(void) const {
    const std::unique_ptr<icomponent_pool>& base_pool = m_Data.at(typeid(Component));
    return *static_cast<const component_pool<Component>*>(base_pool.get());
}

RW_ECS_NAMESPACE_END
#endif