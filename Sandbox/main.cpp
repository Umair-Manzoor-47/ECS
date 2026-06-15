
#include <iostream>
#include <ostream>
#include <Components.h>
#include <ComponentID.h>

int main() {

    std::cout << ecs::ComponentID::get<Transform>() << "\n"; // 0
    std::cout << ecs::ComponentID::get<Velocity>()  << "\n"; // 1
    std::cout << ecs::ComponentID::get<Transform>() << "\n"; // 0
    std::cout << ecs::ComponentID::get<Velocity>()  << "\n"; // 1
    std::cout << ecs::ComponentID::get<Velocity>()  << "\n"; // 1

    return 0;
}
