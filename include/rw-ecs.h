#ifndef RW__ECS__H
#define RW__ECS__H

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <memory>

#ifndef RW_NAMESPACE
    #define RW_NAMESPACE                rw
    #define RW_NAMESPACE_BEGIN          namespace RW_NAMESPACE {
    #define RW_NAMESPACE_END            }
#endif

#ifndef RW_ECS_NAMESPACE
    #define RW_ECS_NAMESPACE            ecs
    #define RW_ECS_NAMESPACE_BEGIN      namespace RW_NAMESPACE::RW_ECS_NAMESPACE {
    #define RW_ECS_NAMESPACE_END        }
#endif

#include "rw-ecs-entity.h"
#include "rw-ecs-entity-manager.h"
#include "rw-ecs-component.h"
#include "rw-ecs-component-list.h"
#include "rw-ecs-component-manager.h"
#include "rw-ecs-system.h"
#include "rw-ecs-system-list.h"
#include "rw-ecs-system-manager.h"
#include "rw-ecs-ecs.h"

#endif