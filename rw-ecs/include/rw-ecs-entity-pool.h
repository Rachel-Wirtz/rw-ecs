#ifndef RW__ECS_ENTITY_POOL__H
#define RW__ECS_ENTITY_POOL__H
RW_ECS_NAMESPACE_BEGIN

constexpr inline entity_handle invalid_entity = std::numeric_limits<entity_handle>::max();

class entity_pool {
public:
    using iterator       = std::vector<entity_handle>::iterator;
    using const_iterator = std::vector<entity_handle>::const_iterator;

    entity_pool() = default;
    entity_pool(entity_pool&&) = default;
    entity_pool& operator=(entity_pool&&) = default;

    void push(entity_handle entity);
    void pop(entity_handle entity);

    bool contains(entity_handle entity) const noexcept;

    entity_handle count(void) const noexcept;

    iterator begin(void) noexcept;
    iterator end(void) noexcept;

    const_iterator begin(void) const noexcept;
    const_iterator end(void) const noexcept;

private:
    std::unordered_map<entity_handle, size_t> m_Indices{};
    std::vector<entity_handle>                m_Data{};

    entity_pool(const entity_pool&) = delete;
    entity_pool& operator=(const entity_pool&) = delete;
};

RW_ECS_NAMESPACE_END
#endif