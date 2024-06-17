#include "rw-ecs-demo.h"
using namespace rw::ecs;

struct NameComponent {
    std::string name;
};

class NameComponentSystem : public component_system<NameComponentSystem> {
public:
    using component_list = std::tuple<NameComponent>;

    NameComponentSystem(const std::string_view user_data)
        : m_UserData{ user_data }
    {
    }

    void update(void) {
        std::span<const entity_handle> entity_list = this->entities();
        entity_component_system* ecs = this->registry();

        for (auto entity : entity_list) {
            NameComponent& name_component = ecs->get_component<NameComponent>(entity);
            std::cout << "Entity #" << entity << " is named \"" << name_component.name << "\"\n";
        }

        std::cout << "UserData: " << m_UserData << '\n';
    }

private:
    std::string m_UserData{};
};

int main() {
    std::unique_ptr<entity_component_system> ecs = std::make_unique<entity_component_system>();
    ecs->register_system<NameComponentSystem>("Greetings, from the main function!");

    while(true) {
        std::string name{};
        std::cout << "How you want to name your next entity?\n";
        std::getline(std::cin, name);

        if (name.empty())
            break;

        entity_handle entity = ecs->create_entity();
        ecs->add_component<NameComponent>(entity, std::move(name));

        std::cout << "\n";
    };

    ecs->get_system<NameComponentSystem>().update();
}