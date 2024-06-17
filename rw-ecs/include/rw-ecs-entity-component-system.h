#ifndef RW__ECS_ENTITY_COMPONENT_SYSTEM__H
#define RW__ECS_ENTITY_COMPONENT_SYSTEM__H
RW_ECS_NAMESPACE_BEGIN

class entity_component_system {
public:
    entity_component_system();

    [[nodiscard]] entity_handle create_entity(void);

    [[nodiscard]] bool validate_entity(entity_handle entity) const noexcept;

    void destroy_entity(entity_handle entity);

    template<typename Component>
    void register_component(void);

    template<typename Component, typename ... Args> requires std::constructible_from<Component, Args...>
    Component& add_component(entity_handle entity, Args&& ... args);

    template<typename Component>
    [[nodiscard]] const Component& get_component(entity_handle entity) const;

    template<typename Component>
    [[nodiscard]] Component& get_component(entity_handle entity);

    template<typename Component>
    [[nodiscard]] bool has_component(entity_handle entity) const noexcept;

    template<typename Component>
    void remove_component(entity_handle entity);


    template<is_user_system UserSystem, typename ... Args> requires std::constructible_from<UserSystem, Args...>
    UserSystem& register_system(Args&& ... args);

    template<is_user_system UserSystem>
    [[nodiscard]] UserSystem& get_system(void);

    template<is_user_system UserSystem>
    [[nodiscard]] bool has_system(void) const noexcept;

private:
    template<is_user_system UserSystem, size_t N = 0>
    void register_system_components(void);

private:
    entity_manager           m_EntityManager;
    component_manager        m_ComponentManager;
    component_system_manager m_SystemManager;
};

template<typename Component>
void entity_component_system::register_component(void) {
    m_ComponentManager.register_component<Component>();
}

template<typename Component, typename ... Args> requires std::constructible_from<Component, Args...>
Component& entity_component_system::add_component(entity_handle entity, Args&& ... args) {
    Component& result = m_ComponentManager.add_component<Component>(entity, std::forward<Args>(args)...);
    m_SystemManager.update_entity(entity);
    return result;
}

template<typename Component>
void entity_component_system::remove_component(entity_handle entity) {
    m_ComponentManager.remove_component<Component>(entity);
    m_SystemManager.update_entity(entity);
}

template<typename Component>
const Component& entity_component_system::get_component(entity_handle entity) const {
    return m_ComponentManager.get_component<Component>(entity);
}

template<typename Component>
Component& entity_component_system::get_component(entity_handle entity) {
    return m_ComponentManager.get_component<Component>(entity);
}

template<typename Component>
bool entity_component_system::has_component(entity_handle entity) const noexcept {
    return m_ComponentManager.has_component<Component>(entity);
}

template<is_user_system UserSystem, typename ... Args> requires std::constructible_from<UserSystem, Args...>
UserSystem& entity_component_system::register_system(Args&& ... args) {
    UserSystem& result = m_SystemManager.register_system<UserSystem>(std::forward<Args>(args)...);
    this->register_system_components<UserSystem>();
    return result;
}

template<is_user_system UserSystem>
UserSystem& entity_component_system::get_system(void) {
    return m_SystemManager.get_system<UserSystem>();
}

template<is_user_system UserSystem>
bool entity_component_system::has_system(void) const noexcept {
    return m_SystemManager.has_system<UserSystem>();
}

template<is_user_system UserSystem, size_t N>
void entity_component_system::register_system_components(void) {
    if constexpr (N != std::tuple_size_v<typename UserSystem::component_list>) {
        using Component = std::tuple_element_t<N, typename UserSystem::component_list>;
        this->register_component<Component>();
        return this->register_system_components<UserSystem, N + 1>();
    }
}

RW_ECS_NAMESPACE_END
#endif