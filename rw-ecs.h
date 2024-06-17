#ifndef RW__ECS__H
#define RW__ECS__H

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <queue>

#ifndef RW_NAMESPACE
    #define RW_NAMESPACE                rw
    #define RW_NAMESPACE_BEGIN          namespace RW_NAMESPACE {
    #define RW_NAMESPACE_END            }
#endif

RW_NAMESPACE_BEGIN

template<std::unsigned_integral Entity>
class basic_entity_component_system;

template<std::unsigned_integral Entity>
class basic_entity_pool;

template<std::unsigned_integral Entity>
class basic_entity_manager;

template<std::unsigned_integral Entity, typename Component>
class basic_component_pool;

template<std::unsigned_integral Entity>
class basic_component_pool_pointer;

template<std::unsigned_integral Entity>
class basic_component_manager;

template<std::unsigned_integral Entity, typename UserSystem>
class basic_system;

template<std::unsigned_integral Entity>
class basic_system_pointer;

template<std::unsigned_integral Entity>
class basic_system_manager;

template<std::unsigned_integral Entity>
class basic_entity_pool {
public:
    using data_type      = std::vector<Entity>;
    using indices_type   = std::unordered_map<Entity, size_t>;
    using iterator       = typename data_type::iterator;
    using const_iterator = typename data_type::const_iterator;

    void push(Entity entity) {
        if (!m_Indices.contains(entity)) {
            m_Indices[entity] = m_Data.size();
            m_Data.push_back(entity);
        }
    }

    void pop(Entity entity) {
        auto it = m_Indices.find(entity);
        if (it == m_Indices.end()) return;

        size_t entity_index = it->second;
        size_t swap_index = m_Data.size() - 1;

        Entity& swap_entity = m_Data[swap_index];
        m_Indices[swap_entity] = entity_index;
        std::swap(swap_entity, m_Data[entity_index]);

        m_Indices.erase(entity);
        m_Data.pop_back();
    }

    bool contains(Entity entity) const noexcept {
        return m_Indices.contains(entity);
    }

    iterator begin(void) noexcept {
        return m_Data.begin();
    }

    iterator end(void) noexcept {
        return m_Data.end();
    }

    const_iterator begin(void) const noexcept {
        return m_Data.begin();
    }

    const_iterator end(void) const noexcept {
        return m_Data.end();
    }

private:
    indices_type m_Indices{};
    data_type    m_Data{};
};

template<std::unsigned_integral Entity>
class basic_entity_manager {
public:
    Entity create_entity(void) {
        Entity result;
        if (m_RemovedIds.size()) {
            result = m_RemovedIds.front();
            m_RemovedIds.pop();
        }
        else {
            result = ++m_MaxId;
        }
        m_Entities.push(result);
        return result;
    }

    void destroy_entity(Entity entity) {
        m_Entities.pop(entity);
        m_RemovedIds.push(entity);
    }

    bool validate_entity(Entity entity) const noexcept {
        return m_Entities.contains(entity);
    }

private:
    basic_entity_pool<Entity> m_Entities{};
    std::queue<Entity>        m_RemovedIds{};
    Entity                    m_MaxId{};
};

template<std::unsigned_integral Entity>
class basic_component_pool_base {
public:
    virtual ~basic_component_pool_base() = default;
    virtual void destroy_entity(Entity entity) = 0;

    friend class basic_component_pool_pointer<Entity>;
};

template<std::unsigned_integral Entity, typename Component>
class basic_component_pool : public basic_component_pool_base<Entity> {
public:
    using data_type      = std::vector<Component>;
    using indices_type   = std::unordered_map<Entity, size_t>;
    using entities_type  = std::unordered_map<size_t, Entity>;
    using iterator       = typename data_type::iterator;
    using const_iterator = typename data_type::const_iterator;

    template<typename ... Args> requires std::constructible_from<Component, Args...>
    Component& push(Entity entity, Args&& ... args) {
        auto it = m_Indices.find(entity);

        if (it != m_Indices.end()) {
            Component& result = m_Components[it->second];
            result = Component(std::forward<Args>(args)...);
            return result;
        }

        size_t new_index = m_Components.size();
        m_Indices[entity] = new_index;
        m_Entities[new_index] = entity;

        return m_Components.emplace_back(std::forward<Args>(args)...);
    }

    void pop(Entity entity) {
        auto it = m_Indices.find(entity);
        if (it == m_Indices.end()) return;

        size_t entity_index = it->second;
        size_t swap_index = m_Components.size() - 1;

        Entity swap_entity = m_Entities[swap_index];
        m_Indices[swap_entity] = entity_index;
        m_Entities[entity_index] = swap_entity;
        std::swap(m_Components[entity_index], m_Components[swap_index]);

        m_Indices.erase(entity);
        m_Entities.erase(entity_index);
        m_Components.pop_back();
    }

    const Component& get(Entity entity) const {
        return m_Components.at(m_Indices.at(entity));
    }

    Component& get(Entity entity) {
        return m_Components.at(m_Indices.at(entity));
    }

    bool contains(Entity entity) const noexcept {
        return m_Indices.contains(entity);
    }

private:
    void destroy_entity(Entity entity) override {
        this->pop(entity);
    }

private:
    indices_type  m_Indices{};
    entities_type m_Entities{};
    data_type     m_Components{};

    friend class basic_component_pool_pointer<Entity>;
};

template<std::unsigned_integral Entity>
class basic_component_pool_pointer {
public:
    template<typename Component>
    basic_component_pool<Entity, Component>& get(void) {
        return *static_cast<basic_component_pool<Entity, Component>*>(m_Ptr.get());
    }

    template<typename Component>
    const basic_component_pool<Entity, Component>& get(void) const {
        return *static_cast<const basic_component_pool<Entity, Component>*>(m_Ptr.get());
    }

    template<typename Component>
    void set(void) {
        m_Ptr = std::make_unique<basic_component_pool<Entity, Component>>();
    }

    basic_component_pool_base<Entity>* operator->() noexcept {
        return m_Ptr.get();
    }

    const basic_component_pool_base<Entity>* operator->() const noexcept {
        return m_Ptr.get();
    }

private:
    std::unique_ptr<basic_component_pool_base<Entity>> m_Ptr{};
};

template<std::unsigned_integral Entity>
class basic_component_manager {
public:
    using data_type = std::unordered_map<std::type_index, basic_component_pool_pointer<Entity>>;

    template<typename Component>
    void register_component(void) {
        if (!m_Data.contains(typeid(Component))) {
            m_Data[typeid(Component)].set<Component>();
        }
    }

    template<typename Component, typename ... Args> requires std::constructible_from<Component, Args...>
    Component& add_component(Entity entity, Args&& ... args) {
        return m_Data.at(typeid(Component)).get<Component>().push(entity, std::forward<Args>(args)...);
    }

    template<typename Component>
    void remove_component(Entity entity) {
        return m_Data.at(typeid(Component)).get<Component>().pop(entity);
    }

    template<typename Component>
    Component& get_component(Entity entity) {
        return m_Data.at(typeid(Component)).get<Component>().get(entity);
    }

    template<typename Component>
    const Component& get_component(Entity entity) const {
        return m_Data.at(typeid(Component)).get<Component>().get(entity);
    }

    template<typename Component>
    bool has_component(Entity entity) const noexcept {
        auto it = m_Data.find(typeid(Component));
        if (it == m_Data.end()) {
            return false;
        }
        return it->second.get<Component>().contains(entity);
    }

private:
    void destroy_entity(Entity entity) {
        for (auto& [type, pointer] : m_Data) {
            pointer->destroy_entity(entity);
        }
    }

private:
    data_type m_Data{};

    friend class basic_entity_component_system<Entity>;
};

template<std::unsigned_integral Entity>
class basic_system_base {
public:
    virtual ~basic_system_base() = default;
    virtual void destroy_entity(Entity entity) = 0;
    virtual void update_entity(Entity entity) = 0;

    const basic_entity_component_system<Entity>* registry(void) const noexcept {
        return m_ECS;
    }

    basic_entity_component_system<Entity>* registry(void) noexcept {
        return m_ECS;
    }

private:
    basic_entity_component_system<Entity>* m_ECS{};

    friend class basic_system_pointer<Entity>;
};

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

template<std::unsigned_integral Entity, typename UserSystem>
class basic_system : public basic_system_base<Entity> {
public:
    const basic_entity_pool<Entity>& entities(void) const noexcept {
        return m_Entities;
    }

    basic_entity_pool<Entity>& entities(void) noexcept {
        return m_Entities;
    }

private:
    void destroy_entity(Entity entity) override {
        m_Entities.pop(entity);
    }

    void update_entity(Entity entity) override {
        if (this->has_components(entity)) {
            m_Entities.push(entity);
        }
        else {
            m_Entities.pop(entity);
        }
    }

    template<size_t N = 0>
    bool has_components(Entity entity) const noexcept {
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

private:
    basic_entity_pool<Entity> m_Entities{};

    friend class basic_system_pointer<Entity>;
};

template<std::unsigned_integral Entity>
class basic_system_pointer {
public:
    template<is_user_system<Entity> UserSystem>
    UserSystem& get(void) {
        return *static_cast<UserSystem*>(m_Ptr.get());
    }

    template<is_user_system<Entity> UserSystem>
    const UserSystem& get(void) const {
        return *static_cast<const UserSystem*>(m_Ptr.get());
    }

    template<is_user_system<Entity> UserSystem, typename ... Args> requires std::constructible_from<UserSystem, Args...>
    UserSystem& set(basic_entity_component_system<Entity>* ecs, Args&& ... args) {
        m_Ptr = std::make_unique<UserSystem>(std::forward<Args>(args)...);
        m_Ptr->m_ECS = ecs;
        return this->get<UserSystem>();
    }

    basic_system_base<Entity>* operator->() noexcept {
        return m_Ptr.get();
    }

    const basic_system_base<Entity>* operator->() const noexcept {
        return m_Ptr.get();
    }

private:
    std::unique_ptr<basic_system_base<Entity>> m_Ptr{};
};

template<std::unsigned_integral Entity>
class basic_system_manager {
public:
    using data_type = std::unordered_map<std::type_index, basic_system_pointer<Entity>>;

    basic_system_manager(basic_entity_component_system<Entity>* ecs)
        : m_Data{}
        , m_ECS{ ecs }
    {
    }

    template<is_user_system<Entity> UserSystem, typename ... Args> requires std::constructible_from<UserSystem, Args...>
    UserSystem& register_system(Args&& ... args) {
        if (this->has_system<UserSystem>()) {
            return this->get_system<UserSystem>();
        }
        basic_system_pointer<Entity>& result = m_Data[typeid(UserSystem)];
        return result.set<UserSystem>(m_ECS, std::forward<Args>(args)...);
    }

    template<is_user_system<Entity> UserSystem>
    UserSystem& get_system(void) {
        return m_Data.at(typeid(UserSystem)).get<UserSystem>();
    }

    template<is_user_system<Entity> UserSystem>
    const UserSystem& get_system(void) const {
        return m_Data.at(typeid(UserSystem)).get<UserSystem>();
    }

    template<is_user_system<Entity> UserSystem>
    bool has_system(void) const noexcept {
        return m_Data.contains(typeid(UserSystem));
    }

private:
    void destroy_entity(Entity entity) {
        for (auto& [type, pointer] : m_Data) {
            pointer->destroy_entity(entity);
        }
    }

    void update_entity(Entity entity) {
        for (auto& [type, pointer] : m_Data) {
            pointer->update_entity(entity);
        }
    }

private:
    data_type                              m_Data{};
    basic_entity_component_system<Entity>* m_ECS{};

    friend class basic_entity_component_system<Entity>;
};

template<std::unsigned_integral Entity>
class basic_entity_component_system {
public:
    using entity_type            = Entity;
    using entity_pool_type       = basic_entity_pool<Entity>;
    using entity_manager_type    = basic_entity_manager<entity_type>;
    using component_manager_type = basic_component_manager<entity_type>;
    using system_manager_type    = basic_system_manager<entity_type>;

    template<is_user_system<Entity> UserSystem>
    using system_type = basic_system<Entity, UserSystem>;

    basic_entity_component_system()
        : m_EntityManager{}
        , m_ComponentManager{}
        , m_SystemManager{ this }
    {
    }

    Entity create_entity(void) {
        return m_EntityManager.create_entity();
    }

    void destroy_entity(Entity entity) {
        m_SystemManager.destroy_entity(entity);
        m_ComponentManager.destroy_entity(entity);
        m_EntityManager.destroy_entity(entity);
    }

    bool validate_entity(Entity entity) const noexcept {
        return m_EntityManager.validate_entity(entity);
    }

    template<typename Component>
    void register_component(void) {
        m_ComponentManager.register_component<Component>();
    }

    template<typename Component, typename ... Args> requires std::constructible_from<Component, Args...>
    Component& add_component(Entity entity, Args&& ... args) {
        Component& result = m_ComponentManager.add_component<Component>(entity, std::forward<Args>(args)...);
        m_SystemManager.update_entity(entity);
        return result;
    }

    template<typename Component>
    void remove_component(Entity entity) {
        m_ComponentManager.remove_component<Component>(entity);
        m_SystemManager.update_entity(entity);
    }

    template<typename Component>
    const Component& get_component(Entity entity) const {
        return m_ComponentManager.get_component<Component>(entity);
    }

    template<typename Component>
    Component& get_component(Entity entity) {
        return m_ComponentManager.get_component<Component>(entity);
    }

    template<typename Component>
    bool has_component(Entity entity) const noexcept {
        return m_ComponentManager.has_component<Component>(entity);
    }

    template<is_user_system<Entity> UserSystem, typename ... Args> requires std::constructible_from<UserSystem, Args...>
    UserSystem& register_system(Args&& ... args) {
        return m_SystemManager.register_system<UserSystem>(std::forward<Args>(args)...);
    }

    template<is_user_system<Entity> UserSystem>
    const UserSystem& get_system(void) const {
        return m_SystemManager.get_system<UserSystem>();
    }

    template<is_user_system<Entity> UserSystem>
    UserSystem& get_system(void) {
        return m_SystemManager.get_system<UserSystem>();
    }

    template<is_user_system<Entity> UserSystem>
    bool has_system(void) const noexcept {
        return m_SystemManager.has_system<UserSystem>();
    }

private:
    entity_manager_type    m_EntityManager;
    component_manager_type m_ComponentManager;
    system_manager_type    m_SystemManager;
};

RW_NAMESPACE_END
#endif