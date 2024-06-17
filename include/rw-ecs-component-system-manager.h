#ifndef RW__ECS_COMPONENT_SYSTEM_MANAGER__H
#define RW__ECS_COMPONENT_SYSTEM_MANAGER__H

#include "rw-ecs.h"

RW_ECS_NAMESPACE_BEGIN

template<std::unsigned_integral Entity>
class basic_component_system_manager {
public:
    using data_type = std::unordered_map<std::type_index, std::unique_ptr<basic_component_system_base<Entity>>>;

    basic_component_system_manager() = delete;
    basic_component_system_manager(basic_entity_component_system<Entity>* ecs);

    template<is_user_system<Entity> UserSystem, typename ... Args> requires std::constructible_from<UserSystem, Args...>
    UserSystem& register_system(Args&& ... args);

    template<is_user_system<Entity> UserSystem>
    UserSystem& get_system(void);

    template<is_user_system<Entity> UserSystem>
    bool has_system(void) const noexcept;

private:
    void destroy_entity(Entity entity);
    void update_entity(Entity entity);

private:
    data_type                              m_Data{};
    basic_entity_component_system<Entity>* m_ECS{};

    friend class basic_entity_component_system<Entity>;
};

template<std::unsigned_integral Entity>
basic_component_system_manager<Entity>::basic_component_system_manager(basic_entity_component_system<Entity>* ecs)
    : m_Data{}
    , m_ECS{ ecs }
{
}

template<std::unsigned_integral Entity>
template<is_user_system<Entity> UserSystem, typename ... Args> requires std::constructible_from<UserSystem, Args...>
UserSystem& basic_component_system_manager<Entity>::register_system(Args&& ... args) {
    if (!m_Data.contains(typeid(UserSystem))) {
        std::unique_ptr<basic_component_system<Entity, UserSystem>> pointer = std::make_unique<UserSystem>(std::forward<Args>(args)...);
        pointer->m_ECS = m_ECS;
        m_Data[typeid(UserSystem)] = std::move(pointer);
    }
    return this->get_system<UserSystem>();
}

template<std::unsigned_integral Entity>
template<is_user_system<Entity> UserSystem>
UserSystem& basic_component_system_manager<Entity>::get_system(void) {
    std::unique_ptr<basic_component_system_base<Entity>>& base_system = m_Data.at(typeid(UserSystem));
    return *static_cast<UserSystem*>(base_system.get());
}

template<std::unsigned_integral Entity>
template<is_user_system<Entity> UserSystem>
bool basic_component_system_manager<Entity>::has_system(void) const noexcept {
    return m_Data.contains(typeid(UserSystem));
}

template<std::unsigned_integral Entity>
void basic_component_system_manager<Entity>::destroy_entity(Entity entity) {
    for (auto& [type, pointer] : m_Data) {
        pointer->destroy_entity(entity);
    }
}

template<std::unsigned_integral Entity>
void basic_component_system_manager<Entity>::update_entity(Entity entity) {
    for (auto& [type, pointer] : m_Data) {
        pointer->update_entity(entity);
    }
}

RW_ECS_NAMESPACE_END

#endif