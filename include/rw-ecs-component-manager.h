#ifndef RW__ECS_COMPONENT_MANAGER__H
#define RW__ECS_COMPONENT_MANAGER__H

#include "rw-ecs.h"

RW_ECS_NAMESPACE_BEGIN

template<typename ComponentList>
class component_manager;

template<typename ... Components>
class component_manager<component_list<Components...>> : public component_pool<Components>... {
public:
    using component_list = component_list<Components...>;

    template<component_list_has<component_list> Component, typename ... Args> requires std::constructible_from<Component, Args...>
    Component& add_component(entity_handle entity, Args&& ... args) {
        component_pool<Component>& pool = static_cast<component_pool<Component>&>(*this);
        return pool.add_component(entity, std::forward<Args>(args)...);
    }

    template<component_list_has<component_list> Component>
    void remove_component(entity_handle entity) {
        component_pool<Component>& pool = static_cast<component_pool<Component>&>(*this);
        pool.remove_component(entity);
    }

    template<component_list_has<component_list> Component>
    Component& get_component(entity_handle entity) {
        component_pool<Component>& pool = static_cast<component_pool<Component>&>(*this);
        return pool.get_component(entity);
    }

    template<component_list_has<component_list> Component>
    bool has_component(entity_handle entity) const noexcept {
        const component_pool<Component>& pool = static_cast<const component_pool<Component>&>(*this);
        return pool.has_component(entity);
    }

    void destroy_entity(entity_handle entity) {
        (this->remove_component<Components>(entity), ...);
    }
};

RW_ECS_NAMESPACE_END
#endif