#ifndef RW__ECS_ECS__H
#define RW__ECS_ECS__H

#include "rw-ecs.h"

RW_ECS_NAMESPACE_BEGIN

template<typename SystemList>
class entity_component_system;

template<typename ... UserSystems>
entity_component_system(UserSystems&...) -> entity_component_system<system_list<UserSystems...>>;

template<typename ... UserSystems> requires (user_system<UserSystems, system_list<UserSystems...>> && ...)
class entity_component_system<system_list<UserSystems...>> {
public:
    using system_list            = system_list<UserSystems...>;
    using system_manager_type    = system_manager<system_list>;
    using component_list         = component_list_cat<typename UserSystems::component_list...>;
    using component_manager_type = component_manager<component_list>;

    entity_component_system(UserSystems& ... systems)
        : m_EntityManager{}
        , m_ComponentManager{}
        , m_SystemManager{ *this, systems... }
    {
    }

    entity_handle create_entity(void) {
        return m_EntityManager.create_entity();
    }

    void destroy_entity(entity_handle entity) {
        m_SystemManager.destroy_entity(entity);
        m_ComponentManager.destroy_entity(entity);
        m_EntityManager.destroy_entity(entity);
    }

    bool validate_entity(entity_handle entity) const noexcept {
        return m_EntityManager.validate_entity(entity);
    }

    template<component_list_has<component_list> Component, typename ... Args> requires std::constructible_from<Component, Args...>
    Component& add_component(entity_handle entity, Args&& ... args) {
        Component& result = m_ComponentManager.add_component<Component>(entity, std::forward<Args>(args)...);
        m_SystemManager.update_entity(entity);
        return result;
    }

    template<component_list_has<component_list> Component>
    Component& get_component(entity_handle entity) {
        return m_ComponentManager.get_component<Component>(entity);
    }

    template<component_list_has<component_list> Component>
    void remove_component(entity_handle entity) {
        m_ComponentManager.remove_component<Component>(entity);
        m_SystemManager.update_entity(entity);
    }

    template<component_list_has<component_list> Component>
    bool has_component(entity_handle entity) const noexcept {
        return m_ComponentManager.has_component<Component>(entity);
    }

    template<system_list_has<system_list> UserSystem>
    void update_system(float time) {
        m_SystemManager.update_system<UserSystem>(time);
    }

    void update_systems(float time) {
        (this->update_system<UserSystems>(time), ...);
    }

private:
    entity_manager         m_EntityManager;
    component_manager_type m_ComponentManager;
    system_manager_type    m_SystemManager;
};

RW_ECS_NAMESPACE_END
#endif