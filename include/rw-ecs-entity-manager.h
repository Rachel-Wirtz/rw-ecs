#ifndef RW__ECS_ENTITY_MANAGER__H
#define RW__ECS_ENTITY_MANAGER__H

#include "rw-ecs.h"

RW_ECS_NAMESPACE_BEGIN

template<std::unsigned_integral Entity>
class basic_entity_manager {
public:
    Entity create_entity(void);
    void   destroy_entity(Entity entity);
    bool   validate_entity(Entity entity) const noexcept;

private:
    basic_entity_pool<Entity> m_Entities{};
    std::queue<Entity>        m_RemovedIds{};
};

template<std::unsigned_integral Entity>
Entity basic_entity_manager<Entity>::create_entity(void) {
    Entity result;
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

template<std::unsigned_integral Entity>
void basic_entity_manager<Entity>::destroy_entity(Entity entity) {
    m_Entities.pop(entity);
    m_RemovedIds.push(entity);
}

template<std::unsigned_integral Entity>
bool basic_entity_manager<Entity>::validate_entity(Entity entity) const noexcept {
    return m_Entities.contains(entity);
}

RW_ECS_NAMESPACE_END
#endif