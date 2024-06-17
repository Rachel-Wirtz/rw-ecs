#ifndef RW__ECS_ENTITY_MANAGER__H
#define RW__ECS_ENTITY_MANAGER__H
RW_ECS_NAMESPACE_BEGIN

class entity_manager {
public:
    entity_handle create_entity(void);
    void destroy_entity(entity_handle entity);
    bool validate_entity(entity_handle entity) const noexcept;

private:
    entity_pool               m_Entities{};
    std::queue<entity_handle> m_RemovedIds{};
};

RW_ECS_NAMESPACE_END
#endif