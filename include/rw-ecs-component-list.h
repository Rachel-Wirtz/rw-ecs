#ifndef RW__ECS_COMPONENT_LIST__H
#define RW__ECS_COMPONENT_LIST__H

#include "rw-ecs.h"

RW_ECS_NAMESPACE_BEGIN

template<typename ... Components>
struct component_list {
    template<typename Component>
    static constexpr bool contains(void) noexcept {
        return (std::same_as<Component, Components> || ...);
    }
};

template<typename Component, typename ComponentList>
concept component_list_has = ComponentList::template contains<Component>();

namespace detail {
    template<typename First, typename Next, typename Done>
    struct unify_component_list_impl;

    template<typename First, typename ... Done>
    struct unify_component_list_impl<First, component_list<>, component_list<Done...>> {
        using type = std::conditional_t<
            (std::same_as<First, Done> || ...),
            component_list<Done...>,
            component_list<Done..., First>
        >;
    };

    template<typename First, typename Next, typename ... Todo, typename ... Done>
    struct unify_component_list_impl<First, component_list<Next, Todo...>, component_list<Done...>>
        : std::conditional_t<
            (std::same_as<First, Done> || ...),
            unify_component_list_impl<Next, component_list<Todo...>, component_list<Done...>>,
            unify_component_list_impl<Next, component_list<Todo...>, component_list<Done..., First>>
        >
    {
    };

    template<typename component_list>
    struct unify_component_list;

    template<typename T>
    struct unify_component_list<component_list<T>> {
        using type = component_list<T>;
    };

    template<typename T, typename ... Ts>
    struct unify_component_list<component_list<T, Ts...>> : unify_component_list_impl<T, component_list<Ts...>, component_list<>>
    {
    };

    template<typename ... component_lists>
    struct concat_component_lists;

    template<typename ... Ts>
    struct concat_component_lists<component_list<Ts...>> {
        using type = typename unify_component_list<component_list<Ts...>>::type;
    };

    template<typename ... Ls, typename ... Rs>
    struct concat_component_lists<component_list<Ls...>, component_list<Rs...>> {
        using type = typename unify_component_list<component_list<Ls..., Rs...>>::type;
    };
}

template<typename ... ComponentLists>
using component_list_cat = typename detail::concat_component_lists<ComponentLists...>::type;

RW_ECS_NAMESPACE_END
#endif