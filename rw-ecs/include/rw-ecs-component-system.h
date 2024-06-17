#ifndef RW__ECS_COMPONENT_SYSTEM__H
#define RW__ECS_COMPONENT_SYSTEM__H
RW_ECS_NAMESPACE_BEGIN

class entity_component_system;

class icomponent_system {
public:
    virtual ~icomponent_system() = default;
    virtual void destroy_entity(entity_handle entity) = 0;
    virtual void update_entity(entity_handle entity) = 0;
};

template<typename UserSystem>
class component_system : public icomponent_system {
public:
    std::span<const entity_handle> entities(void) const noexcept;

    const entity_component_system* registry(void) const noexcept;
    entity_component_system* registry(void) noexcept;

private:
    void destroy_entity(entity_handle entity) override;
    void update_entity(entity_handle entity) override;

    template<size_t N = 0>
    bool has_components(entity_handle entity) const noexcept;

private:
    entity_pool              m_Entities{};
    entity_component_system* m_ECS{};

    friend class component_system_manager;
};

template<typename UserSystem>
std::span<const entity_handle> component_system<UserSystem>::entities(void) const noexcept {
    return { m_Entities.begin(), m_Entities.end() };
}

template<typename UserSystem>
entity_component_system* component_system<UserSystem>::registry(void) noexcept {
    return m_ECS;
}

template<typename UserSystem>
const entity_component_system* component_system<UserSystem>::registry(void) const noexcept {
    return m_ECS;
}

template<typename UserSystem>
void component_system<UserSystem>::destroy_entity(entity_handle entity) {
    m_Entities.pop(entity);
}

template<typename UserSystem>
void component_system<UserSystem>::update_entity(entity_handle entity) {
    if (this->has_components(entity)) {
        m_Entities.push(entity);
    }
    else {
        m_Entities.pop(entity);
    }
}

template<typename UserSystem>
template<size_t N>
bool component_system<UserSystem>::has_components(entity_handle entity) const noexcept {
    if constexpr (N == std::tuple_size_v<typename UserSystem::component_list>) {
        return true;
    }
    else {
        using Component = std::tuple_element_t<N, typename UserSystem::component_list>;
        if (!m_ECS->has_component<Component>(entity)) {
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

template<typename UserSystem>
concept is_user_system = std::derived_from<UserSystem, component_system<UserSystem>> && is_component_list<typename UserSystem::component_list>;

RW_ECS_NAMESPACE_END
#endif