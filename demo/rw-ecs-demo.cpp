#include "rw-ecs-demo.h"
#include <iostream>

void NameComponentSystem::update(void) {
    std::span<const entity_handle> entity_list = this->entities();
    entity_component_system*       ecs         = this->registry();

    for (entity_handle entity : entity_list) {
        NameComponent& name_component = ecs->get_component<NameComponent>(entity);
        std::cout << "Entity #" << entity << " is named \"" << name_component.name << "\"\n";
    }
}

int main() {
    entity_component_system ecs{};
    NameComponentSystem& name_system = ecs.register_system<NameComponentSystem>();

    while(true) {
        std::string name{};
        std::cout << "How you want to name your next entity?\n";
        std::getline(std::cin, name);

        if (name.empty())
            break;

        entity_handle entity = ecs.create_entity();
        ecs.add_component<NameComponent>(entity, std::move(name));
        std::cout << "\n";
    };

    name_system.update();
}