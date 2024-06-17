#ifndef RW__ECS_ENTITY_COMPONENT_SYSTEM__H
#define RW__ECS_ENTITY_COMPONENT_SYSTEM__H

#include "rw-ecs.h"

RW_ECS_NAMESPACE_BEGIN

template<std::unsigned_integral Entity>
class basic_entity_component_system {
public:
    using entity_manager_type    = basic_entity_manager<Entity>;
    using component_manager_type = basic_component_manager<Entity>;
    using system_manager_type    = basic_component_system_manager<Entity>;

    basic_entity_component_system();

    [[nodiscard]] Entity create_entity(void);

    [[nodiscard]] bool validate_entity(Entity entity) const noexcept;

    void destroy_entity(Entity entity);

    template<typename Component>
    void register_component(void);

    template<typename Component, typename ... Args> requires std::constructible_from<Component, Args...>
    Component& add_component(Entity entity, Args&& ... args);

    template<typename Component>
    [[nodiscard]] const Component& get_component(Entity entity) const;

    template<typename Component>
    [[nodiscard]] Component& get_component(Entity entity);

    template<typename Component>
    [[nodiscard]] bool has_component(Entity entity) const noexcept;

    template<typename Component>
    void remove_component(Entity entity);


    template<is_user_system<Entity> UserSystem, typename ... Args> requires std::constructible_from<UserSystem, Args...>
    UserSystem& register_system(Args&& ... args);

    template<is_user_system<Entity> UserSystem>
    [[nodiscard]] UserSystem& get_system(void);

    template<is_user_system<Entity> UserSystem>
    [[nodiscard]] bool has_system(void) const noexcept;

private:
    template<is_user_system<Entity> UserSystem, size_t N = 0>
    void register_system_components(void);

private:
    entity_manager_type    m_EntityManager;
    component_manager_type m_ComponentManager;
    system_manager_type    m_SystemManager;
};

template<std::unsigned_integral Entity>
basic_entity_component_system<Entity>::basic_entity_component_system()
    : m_EntityManager{}
    , m_ComponentManager{}
    , m_SystemManager{ this }
{
}

template<std::unsigned_integral Entity>
Entity basic_entity_component_system<Entity>::create_entity(void) {
    return m_EntityManager.create_entity();
}

template<std::unsigned_integral Entity>
void basic_entity_component_system<Entity>::destroy_entity(Entity entity) {
    m_SystemManager.destroy_entity(entity);
    m_ComponentManager.destroy_entity(entity);
    m_EntityManager.destroy_entity(entity);
}

template<std::unsigned_integral Entity>
bool basic_entity_component_system<Entity>::validate_entity(Entity entity) const noexcept {
    return m_EntityManager.validate_entity(entity);
}

template<std::unsigned_integral Entity>
template<typename Component>
void basic_entity_component_system<Entity>::register_component(void) {
    m_ComponentManager.register_component<Component>();
}

template<std::unsigned_integral Entity>
template<typename Component, typename ... Args> requires std::constructible_from<Component, Args...>
Component& basic_entity_component_system<Entity>::add_component(Entity entity, Args&& ... args) {
    Component& result = m_ComponentManager.add_component<Component>(entity, std::forward<Args>(args)...);
    m_SystemManager.update_entity(entity);
    return result;
}

template<std::unsigned_integral Entity>
template<typename Component>
void basic_entity_component_system<Entity>::remove_component(Entity entity) {
    m_ComponentManager.remove_component<Component>(entity);
    m_SystemManager.update_entity(entity);
}

template<std::unsigned_integral Entity>
template<typename Component>
const Component& basic_entity_component_system<Entity>::get_component(Entity entity) const {
    return m_ComponentManager.get_component<Component>(entity);
}

template<std::unsigned_integral Entity>
template<typename Component>
Component& basic_entity_component_system<Entity>::get_component(Entity entity) {
    return m_ComponentManager.get_component<Component>(entity);
}

template<std::unsigned_integral Entity>
template<typename Component>
bool basic_entity_component_system<Entity>::has_component(Entity entity) const noexcept {
    return m_ComponentManager.has_component<Component>(entity);
}

template<std::unsigned_integral Entity>
template<is_user_system<Entity> UserSystem, typename ... Args> requires std::constructible_from<UserSystem, Args...>
UserSystem& basic_entity_component_system<Entity>::register_system(Args&& ... args) {
    UserSystem& result = m_SystemManager.register_system<UserSystem>(std::forward<Args>(args)...);
    this->register_system_components<UserSystem>();
    return result;
}

template<std::unsigned_integral Entity>
template<is_user_system<Entity> UserSystem>
UserSystem& basic_entity_component_system<Entity>::get_system(void) {
    return m_SystemManager.get_system<UserSystem>();
}

template<std::unsigned_integral Entity>
template<is_user_system<Entity> UserSystem>
bool basic_entity_component_system<Entity>::has_system(void) const noexcept {
    return m_SystemManager.has_system<UserSystem>();
}

template<std::unsigned_integral Entity>
template<is_user_system<Entity> UserSystem, size_t N>
void basic_entity_component_system<Entity>::register_system_components(void) {
    if constexpr (N != std::tuple_size_v<typename UserSystem::component_list>) {
        using Component = std::tuple_element_t<N, typename UserSystem::component_list>;
        this->register_component<Component>();
        return this->register_system_components<UserSystem, N + 1>();
    }
}

RW_ECS_NAMESPACE_END
#endif