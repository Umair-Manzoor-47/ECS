
#include <iostream>
#include <ostream>
#include <Components.h>
#include <ComponentColumn.h>
#include <ComponentID.h>

int main() {

    std::cout << "--- creating column ---\n";
    auto col = ecs::ComponentColumn::Create<LoudTransform>();

    std::cout << "--- pushing 5 elements (should trigger 1 grow at the 5th) ---\n";
    col.PushBack(LoudTransform{1, 1});
    col.PushBack(LoudTransform{2, 2});
    col.PushBack(LoudTransform{3, 3});
    col.PushBack(LoudTransform{4, 4});
    col.PushBack(LoudTransform{5, 5}); // capacity 4 -> 8 here

    std::cout << "--- end of scope, destructor should run ---\n";
    return 0;
}
