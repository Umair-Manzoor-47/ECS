
#include <iostream>
#include <ostream>
#include <Components.h>
#include <ComponentColumn.h>
#include <ComponentID.h>
#include <Archetype.h>

void TestArchetype();

int main() {

    TestArchetype();
    return 0;
}

void TestComponentColumn() {
    std::cout << "--- creating column ---\n";
    auto col = ecs::ComponentColumn::Create<LoudTransform>();

    std::cout << "--- pushing 5 elements (should trigger 1 grow at the 5th) ---\n";
    col.PushBack(LoudTransform{1, 1});
    col.PushBack(LoudTransform{2, 2});
    col.PushBack(LoudTransform{3, 3});
    col.PushBack(LoudTransform{4, 4});
    col.PushBack(LoudTransform{5, 5}); // capacity 4 -> 8 here

    std::cout << "--- end of scope, destructor should run ---\n";
}

void TestArchetype() {
    auto arch = ecs::Archetype({
    ecs::ComponentID::get<Transform>(),
    ecs::ComponentID::get<Velocity>()
    });

    arch.AddColumn<Transform>();
    arch.AddColumn<Velocity>();

    arch.PushBack(Transform{1.0f, 2.0f}, Velocity{3.0f, 4.0f});
    arch.PushBack(Transform{5.0f, 6.0f}, Velocity{7.0f, 8.0f});

    // expect 1, 2
    auto& t = arch.GetComponent<Transform>(0);
    std::cout << t.x << ", " << t.y << "\n";

    // expect 1, 2
    t.x = 60.0f;
    t.y = 1.0f;
    std::cout << arch.GetComponent<Transform>(0).x << "\n";

    arch.RemoveRow(0);
    std::cout << arch.GetComponent<Transform>(0).x << "\n";
}