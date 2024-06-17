#ifndef RW__ECS_SYSTEM__H
#define RW__ECS_SYSTEM__H

#include "rw-ecs.h"

RW_ECS_NAMESPACE_BEGIN

template<std::unsigned_integral Entity>
class basic_system_base {
public:
    virtual ~basic_system_base() = default;
    virtual void destroy_entity(Entity entity) = 0;
    virtual void update_entity(Entity entity) = 0;
};

template<std::unsigned_integral Entity, typename UserSystem>
class basic_system : public basic_system_base<Entity> {
public:
    std::span<const Entity> entities(void) const noexcept;

    const basic_entity_component_system<Entity>* registry(void) const noexcept;
    basic_entity_component_system<Entity>* registry(void) noexcept;

private:
    void destroy_entity(Entity entity) override;
    void update_entity(Entity entity) override;

    template<size_t N = 0>
    bool has_components(Entity entity) const noexcept;

private:
    basic_entity_pool<Entity>              m_Entities{};
    basic_entity_component_system<Entity>* m_ECS{};

    friend class basic_system_manager<Entity>;
};

template<std::unsigned_integral Entity, typename UserSystem>
std::span<const Entity> basic_system<Entity, UserSystem>::entities(void) const noexcept {
    return { m_Entities.begin(), m_Entities.end() };
}

template<std::unsigned_integral Entity, typename UserSystem>
basic_entity_component_system<Entity>* basic_system<Entity, UserSystem>::registry(void) noexcept {
    return m_ECS;
}

template<std::unsigned_integral Entity, typename UserSystem>
const basic_entity_component_system<Entity>* basic_system<Entity, UserSystem>::registry(void) const noexcept {
    return m_ECS;
}

template<std::unsigned_integral Entity, typename UserSystem>
void basic_system<Entity, UserSystem>::destroy_entity(Entity entity) {
    m_Entities.pop(entity);
}

template<std::unsigned_integral Entity, typename UserSystem>
void basic_system<Entity, UserSystem>::update_entity(Entity entity) {
    if (this->has_components(entity)) {
        m_Entities.push(entity);
    }
    else {
        m_Entities.pop(entity);
    }
}

template<std::unsigned_integral Entity, typename UserSystem>
template<size_t N>
bool basic_system<Entity, UserSystem>::has_components(Entity entity) const noexcept {
    if constexpr (N == std::tuple_size_v<typename UserSystem::component_list>) {
        return true;
    }
    else {
        using Component = std::tuple_element_t<N, typename UserSystem::component_list>;
        if (!this->registry()->has_component<Component>(entity)) {
            return false;
        }
        return this->has_components<N + 1>(entity);
    }
}

namespace detail {
    template<typename T>
    struct is_component_list : std::integral_constant<bool, false>
    {
    };

    template<typename ... Components>
    struct is_component_list<std::tuple<Components...>> : std::integral_constant<bool, true>
    {
    };
}

template<typename T>
concept is_component_list = detail::is_component_list<T>::value;

template<typename UserSystem, typename Entity>
concept is_user_system = std::derived_from<UserSystem, basic_system<Entity, UserSystem>>&& is_component_list<typename UserSystem::component_list>;

RW_ECS_NAMESPACE_END
#endif