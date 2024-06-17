#ifndef RW__ECS_ENTITY_MANAGER__H
#define RW__ECS_ENTITY_MANAGER__H
RW_ECS_NAMESPACE_BEGIN

class entity_manager {
public:
    entity_manager() = default;
    entity_manager(entity_manager&&) = default;
    entity_manager& operator=(entity_manager&&) = default;

    entity_handle create_entity(void);
    void destroy_entity(entity_handle entity);
    bool validate_entity(entity_handle entity) const noexcept;

private:
    entity_pool               m_Entities{};
    std::queue<entity_handle> m_RemovedIds{};

    entity_manager(const entity_manager&) = delete;
    entity_manager& operator=(const entity_manager&) = delete;
};

RW_ECS_NAMESPACE_END
#endif