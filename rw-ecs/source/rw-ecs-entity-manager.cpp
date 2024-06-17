#include "rw-ecs.h"
RW_ECS_NAMESPACE_BEGIN

entity_handle entity_manager::create_entity(void) {
    entity_handle result;
    if (m_RemovedIds.size()) {
        result = m_RemovedIds.front();
        m_RemovedIds.pop();
    }
    else {
        result = m_Entities.count();
    }
    m_Entities.push(result);
    return result;
}

void entity_manager::destroy_entity(entity_handle entity) {
    m_Entities.pop(entity);
    m_RemovedIds.push(entity);
}

bool entity_manager::validate_entity(entity_handle entity) const noexcept {
    return m_Entities.contains(entity);
}

RW_ECS_NAMESPACE_END