#include "rw-ecs.h"
RW_ECS_NAMESPACE_BEGIN

component_system_manager::component_system_manager(entity_component_system* ecs)
    : m_Data{}
    , m_ECS{ ecs }
{
}

void component_system_manager::destroy_entity(entity_handle entity) {
    for (auto& [type, pointer] : m_Data) {
        pointer->destroy_entity(entity);
    }
}

void component_system_manager::update_entity(entity_handle entity) {
    for (auto& [type, pointer] : m_Data) {
        pointer->update_entity(entity);
    }
}

RW_ECS_NAMESPACE_END