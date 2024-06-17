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

// Adjust me as needed:
using entity_handle = uint32_t;

RW_ECS_NAMESPACE_END

#include "rw-ecs-entity-pool.h"
#include "rw-ecs-entity-manager.h"
#include "rw-ecs-component-pool.h"
#include "rw-ecs-component-manager.h"
#include "rw-ecs-component-system.h"
#include "rw-ecs-component-system-manager.h"
#include "rw-ecs-entity-component-system.h"

#endif