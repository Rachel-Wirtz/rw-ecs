#include "rw-ecs.h"
RW_ECS_NAMESPACE_BEGIN

entity_component_system::entity_component_system()
    : m_EntityManager{}
    , m_ComponentManager{}
    , m_SystemManager{ this }
{
}

entity_handle entity_component_system::create_entity(void) {
    return m_EntityManager.create_entity();
}

void entity_component_system::destroy_entity(entity_handle entity) {
    m_SystemManager.destroy_entity(entity);
    m_ComponentManager.destroy_entity(entity);
    m_EntityManager.destroy_entity(entity);
}

bool entity_component_system::validate_entity(entity_handle entity) const noexcept {
    return m_EntityManager.validate_entity(entity);
}



RW_ECS_NAMESPACE_END