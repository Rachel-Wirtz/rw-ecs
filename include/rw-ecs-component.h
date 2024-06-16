#ifndef RW__ECS_COMPONENT__H
#define RW__ECS_COMPONENT__H

#include "rw-ecs.h"

RW_ECS_NAMESPACE_BEGIN

template<typename Component>
class component_pool {
public:
    template<typename ... Args> requires std::constructible_from<Component, Args...>
    Component& add_component(entity_handle entity, Args&& ... args) {
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

    void remove_component(entity_handle entity) {
        auto it = m_Indices.find(entity);
        if (it == m_Indices.end()) return;

        size_t entity_index = it->second;
        size_t swap_index = m_Components.size() - 1;

        if (entity_index == swap_index) {
            m_Indices.erase(entity);
            m_Entities.erase(entity_index);
            m_Components.pop_back();
            return;
        }

        entity_handle swap_entity = m_Entities[swap_index];
        m_Indices[swap_entity] = entity_index;
        m_Entities[entity_index] = swap_entity;
        std::swap(m_Components[entity_index], m_Components[swap_index]);

        m_Indices.erase(entity);
        m_Entities.erase(entity_index);
        m_Components.pop_back();
    }

    Component& get_component(entity_handle entity) {
        return m_Components.at(m_Indices.at(entity));
    }

    bool has_component(entity_handle entity) const noexcept {
        return m_Indices.contains(entity);
    }

private:
    std::unordered_map<entity_handle, size_t> m_Indices{};
    std::unordered_map<size_t, entity_handle> m_Entities{};
    std::vector<Component>                    m_Components{};
};

RW_ECS_NAMESPACE_END
#endif