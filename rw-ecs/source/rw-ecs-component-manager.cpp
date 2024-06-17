#include "rw-ecs.h"
RW_ECS_NAMESPACE_BEGIN

void component_manager::destroy_entity(entity_handle entity) {
    for (auto& [type, pointer] : m_Data) {
        pointer->destroy_entity(entity);
    }
}

RW_ECS_NAMESPACE_END