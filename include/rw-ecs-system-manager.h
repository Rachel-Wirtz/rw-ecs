#ifndef RW__ECS_SYSTEM_MANAGER__H
#define RW__ECS_SYSTEM_MANAGER__H

#include "rw-ecs.h"

RW_ECS_NAMESPACE_BEGIN

template<typename SystemList>
class system_manager;

template<typename ... UserSystems> requires (user_system<UserSystems, system_list<UserSystems...>> && ...)
class system_manager<system_list<UserSystems...>> : public system<UserSystems, typename UserSystems::component_list>... {
public:
    using system_list = system_list<UserSystems...>;

    system_manager(entity_component_system<system_list>& ecs, UserSystems& ... systems)
        : system<UserSystems, typename UserSystems::component_list>(systems)...
        , m_ECS{ ecs }
    {
    }

    void update_entity(entity_handle entity) {
        (this->update_system_entity<UserSystems>(entity), ...);
    }

    void destroy_entity(entity_handle entity) {
        (this->destroy_system_entity<UserSystems>(entity), ...);
    }

    void update(float time) {
        (this->update<UserSystems>(time), ...);
    }

    template<system_list_has<system_list> UserSystem>
    void update_system(float time) {
        auto& self = static_cast<system<UserSystem, typename UserSystem::component_list>&>(*this);
        self.update(m_ECS, time);
    }

    template<system_list_has<system_list> UserSystem>
    void update_system_entity(entity_handle entity) {
        auto& self = static_cast<system<UserSystem, typename UserSystem::component_list>&>(*this);
        self.update_entity(m_ECS, entity);
    }

    template<system_list_has<system_list> UserSystem>
    void destroy_system_entity(entity_handle entity) {
        auto& self = static_cast<system<UserSystem, typename UserSystem::component_list>&>(*this);
        self.destroy_entity(entity);
    }

private:
    entity_component_system<system_list>& m_ECS;
};

RW_ECS_NAMESPACE_END

#endif