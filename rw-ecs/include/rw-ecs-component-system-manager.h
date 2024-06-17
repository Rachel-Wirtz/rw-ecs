#ifndef RW__ECS_COMPONENT_SYSTEM_MANAGER__H
#define RW__ECS_COMPONENT_SYSTEM_MANAGER__H
RW_ECS_NAMESPACE_BEGIN

class component_system_manager {
public:
    component_system_manager() = default;
    component_system_manager(entity_component_system* ecs);
    component_system_manager(component_system_manager&&) = default;
    component_system_manager& operator=(component_system_manager&&) = default;

    template<is_user_system UserSystem, typename ... Args> requires std::constructible_from<UserSystem, Args...>
    UserSystem& register_system(Args&& ... args);

    template<is_user_system UserSystem>
    UserSystem& get_system(void);

    template<is_user_system UserSystem>
    bool has_system(void) const noexcept;

private:
    void destroy_entity(entity_handle entity);
    void update_entity(entity_handle entity);

private:
    std::unordered_map<std::type_index, std::unique_ptr<icomponent_system>> m_Data{};
    entity_component_system*                                                m_ECS{};

    component_system_manager(const component_system_manager&) = delete;
    component_system_manager& operator=(const component_system_manager&) = delete;

    friend class entity_component_system;
};

template<is_user_system UserSystem, typename ... Args> requires std::constructible_from<UserSystem, Args...>
UserSystem& component_system_manager::register_system(Args&& ... args) {
    if (!m_Data.contains(typeid(UserSystem))) {
        std::unique_ptr<component_system<UserSystem>> pointer = std::make_unique<UserSystem>(std::forward<Args>(args)...);
        pointer->m_ECS = m_ECS;
        m_Data[typeid(UserSystem)] = std::move(pointer);
    }
    return this->get_system<UserSystem>();
}

template<is_user_system UserSystem>
UserSystem& component_system_manager::get_system(void) {
    std::unique_ptr<icomponent_system>& base_system = m_Data.at(typeid(UserSystem));
    return *static_cast<UserSystem*>(base_system.get());
}

template<is_user_system UserSystem>
bool component_system_manager::has_system(void) const noexcept {
    return m_Data.contains(typeid(UserSystem));
}

RW_ECS_NAMESPACE_END
#endif