#ifndef RW__ECS_ENTITY_MANAGER__H
#define RW__ECS_ENTITY_MANAGER__H

#include "rw-ecs.h"

RW_ECS_NAMESPACE_BEGIN

class entity_manager {
public:
    entity_handle create_entity(void) {
        m_Entities.add_entity(++m_LastHandle);
        return m_LastHandle;
    }

    void destroy_entity(entity_handle entity) {
        m_Entities.remove_entity(entity);
    }

    bool validate_entity(entity_handle entity) const noexcept {
        return m_Entities.contains_entity(entity);
    }

private:
    entity_pool   m_Entities{};
    entity_handle m_LastHandle{};
};

RW_ECS_NAMESPACE_END

#endif