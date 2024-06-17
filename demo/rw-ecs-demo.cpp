#include "rw-ecs-demo.h"


struct NameComponent {
    std::string name;
};

class NameComponentSystem : public rw::ecs::component_system<NameComponentSystem> {
public:
    using component_list = std::tuple<NameComponent>;

    void update(void) {
        std::span<const rw::ecs::entity_handle> entity_list = this->entities();
        rw::ecs::entity_component_system* ecs = this->registry();

        for (auto entity : entity_list) {
            NameComponent& name_component = ecs->get_component<NameComponent>(entity);
            std::cout << "Entity #" << entity << " is named \"" << name_component.name << "\"\n";
        }
    }
};

int main() {
    rw::ecs::entity_component_system ecs{};
    ecs.register_system<NameComponentSystem>();

    while(true) {
        std::string name{};
        std::cout << "How you want to name your next entity?\n";
        std::getline(std::cin, name);

        if (name.empty())
            break;

        rw::ecs::entity_handle entity = ecs.create_entity();
        ecs.add_component<NameComponent>(entity, std::move(name));

        std::cout << "\n";
    };

    ecs.get_system<NameComponentSystem>().update();
}