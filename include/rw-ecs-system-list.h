#ifndef RW__ECS_SYSTEM_LIST__H
#define RW__ECS_SYSTEM_LIST__H

#include "rw-ecs.h"

RW_ECS_NAMESPACE_BEGIN

template<typename System, typename SystemList>
concept user_system = requires(System& system, entity_component_system<SystemList>& ecs, entity_pool& entities, float time) {
    { system(ecs, entities, time) } -> std::same_as<void>;
};

template<typename ... Systems>
struct system_list {
    using component_list = component_list_cat<typename Systems::component_list...>;

    template<typename System>
    static constexpr bool contains(void) noexcept {
        return (std::same_as<System, Systems> || ...);
    }
};

template<typename System, typename SystemList>
concept system_list_has = SystemList::template contains<System>();

RW_ECS_NAMESPACE_END
#endif