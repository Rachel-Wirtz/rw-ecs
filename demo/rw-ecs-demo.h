#ifndef RW__ECS_DEMO__H
#define RW__ECS_DEMO__H

#include "rw-ecs.h"
#include <string>

using entity_handle = uint32_t;

template<typename UserSystem>
using component_system = rw::ecs::basic_component_system<entity_handle, UserSystem>;

using entity_component_system = rw::ecs::basic_entity_component_system<entity_handle>;

struct NameComponent {
    std::string name;
};

class NameComponentSystem : public component_system<NameComponentSystem> {
public:
    using component_list = std::tuple<NameComponent>;
    void update(void);
};

#endif