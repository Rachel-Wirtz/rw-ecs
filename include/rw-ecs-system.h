#ifndef RW__ECS_SYSTEM__H
#define RW__ECS_SYSTEM__H

#include "rw-ecs.h"

RW_ECS_NAMESPACE_BEGIN

template<typename SystemList>
class entity_component_system;

template<typename UserSystem, typename ComponentList>
class system;

template<typename UserSystem, typename ... Components>
class system<UserSystem, component_list<Components...>> {
public:
    using component_list = component_list<Components...>;

    system(UserSystem& system)
        : m_Entities{}
        , m_System{ system }
    {
    }

    template<typename SystemList>
    void update(entity_component_system<SystemList>& ecs, float time) {
        m_System(ecs, m_Entities, time);
    }

    template<typename SystemList>
    void update_entity(entity_component_system<SystemList>& ecs, entity_handle entity) {
        if ((ecs.has_component<Components>(entity) && ...)) {
            m_Entities.add_entity(entity);
        }
        else {
            m_Entities.remove_entity(entity);
        }
    }

    void destroy_entity(entity_handle entity) {
        m_Entities.remove_entity(entity);
    }

    entity_pool& entities(void) noexcept {
        return m_Entities;
    }

    const entity_pool& entities(void) const noexcept {
        return m_Entities;
    }

private:
    entity_pool m_Entities;
    UserSystem& m_System;
};

RW_ECS_NAMESPACE_END
#endif