#ifndef RW__ECS_COMPONENT_MANAGER__H
#define RW__ECS_COMPONENT_MANAGER__H

#include "rw-ecs.h"

RW_ECS_NAMESPACE_BEGIN

template<std::unsigned_integral Entity>
class basic_component_manager {
public:
    using data_type = std::unordered_map<std::type_index, std::unique_ptr<basic_component_pool_base<Entity>>>;

    template<typename Component>
    void register_component(void);

    template<typename Component, typename ... Args> requires std::constructible_from<Component, Args...>
    Component& add_component(Entity entity, Args&& ... args);

    template<typename Component>
    void remove_component(Entity entity);

    template<typename Component>
    Component& get_component(Entity entity);

    template<typename Component>
    bool has_component(Entity entity) const noexcept;

private:
    template<typename Component>
    basic_component_pool<Entity, Component>& get_pool(void);

    template<typename Component>
    const basic_component_pool<Entity, Component>& get_pool(void) const;

    void destroy_entity(Entity entity);

private:
    data_type m_Data{};

    friend class basic_entity_component_system<Entity>;
};

template<std::unsigned_integral Entity>
template<typename Component>
void basic_component_manager<Entity>::register_component(void) {
    if (m_Data.contains(typeid(Component))) return;
    m_Data[typeid(Component)] = std::make_unique<basic_component_pool<Entity, Component>>();
}

template<std::unsigned_integral Entity>
template<typename Component, typename ... Args> requires std::constructible_from<Component, Args...>
Component& basic_component_manager<Entity>::add_component(Entity entity, Args&& ... args) {
    basic_component_pool<Entity, Component>& pool = this->get_pool<Component>();
    return pool.push(entity, std::forward<Args>(args)...);
}

template<std::unsigned_integral Entity>
template<typename Component>
void basic_component_manager<Entity>::remove_component(Entity entity) {
    basic_component_pool<Entity, Component>& pool = this->get_pool<Component>();
    pool.pop(entity);
}

template<std::unsigned_integral Entity>
template<typename Component>
Component& basic_component_manager<Entity>::get_component(Entity entity) {
    basic_component_pool<Entity, Component>& pool = this->get_pool<Component>();
    return pool.get(entity);
}

template<std::unsigned_integral Entity>
template<typename Component>
bool basic_component_manager<Entity>::has_component(Entity entity) const noexcept {
    if (!m_Data.contains(typeid(Component))) return false;
    const basic_component_pool<Entity, Component>& pool = this->get_pool<Component>();
    return pool.contains(entity);
}

template<std::unsigned_integral Entity>
template<typename Component>
basic_component_pool<Entity, Component>& basic_component_manager<Entity>::get_pool(void) {
    std::unique_ptr<basic_component_pool_base<Entity>>& base_pool = m_Data.at(typeid(Component));
    return *static_cast<basic_component_pool<Entity, Component>*>(base_pool.get());
}

template<std::unsigned_integral Entity>
template<typename Component>
const basic_component_pool<Entity, Component>& basic_component_manager<Entity>::get_pool(void) const {
    const std::unique_ptr<basic_component_pool_base<Entity>>& base_pool = m_Data.at(typeid(Component));
    return *static_cast<const basic_component_pool<Entity, Component>*>(base_pool.get());
}

template<std::unsigned_integral Entity>
void basic_component_manager<Entity>::destroy_entity(Entity entity) {
    for (auto& [type, pointer] : m_Data) {
        pointer->destroy_entity(entity);
    }
}

RW_ECS_NAMESPACE_END
#endif