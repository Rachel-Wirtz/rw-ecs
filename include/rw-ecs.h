#ifndef RW__ECS__H
#define RW__ECS__H

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <queue>
#include <span>
#include <limits>
#include <cassert>

#ifndef RW_NAMESPACE
    #define RW_NAMESPACE                    rw
    #define RW_NAMESPACE_BEGIN              namespace RW_NAMESPACE {
    #define RW_NAMESPACE_END                }
#endif

#ifndef RW_ECS_NAMESPACE
    #define RW_ECS_NAMESPACE                ecs
    #define RW_ECS_NAMESPACE_BEGIN          namespace RW_NAMESPACE::RW_ECS_NAMESPACE {
    #define RW_ECS_NAMESPACE_END            }
#endif

RW_ECS_NAMESPACE_BEGIN

template<std::unsigned_integral Entity>
constexpr inline Entity invalid_entity = std::numeric_limits<Entity>::max();

template<std::unsigned_integral Entity>
class basic_entity_pool;

template<std::unsigned_integral Entity>
class basic_entity_manager;

template<std::unsigned_integral Entity, typename Component>
class basic_component_pool;

template<std::unsigned_integral Entity>
class basic_component_manager;

template<std::unsigned_integral Entity, typename UserSystem>
class basic_component_system;

template<std::unsigned_integral Entity>
class basic_component_system_manager;

template<std::unsigned_integral Entity>
class basic_entity_component_system;

RW_ECS_NAMESPACE_END

#include "rw-ecs-entity-pool.h"
#include "rw-ecs-entity-manager.h"
#include "rw-ecs-component-pool.h"
#include "rw-ecs-component-manager.h"
#include "rw-ecs-component-system.h"
#include "rw-ecs-component-system-manager.h"
#include "rw-ecs-entity-component-system.h"

#endif