#include <iostream>
#include <Components.h>
#include <ComponentColumn.h>
#include <ComponentID.h>
#include <Archetype.h>
#include <World.h>

// ─────────────────────────────────────────────
// Minimal test harness
// ─────────────────────────────────────────────

static int s_passed = 0;
static int s_failed = 0;

#define EXPECT_EQ(actual, expected, label)                                      \
    if ((actual) == (expected)) {                                               \
        std::cout << "  [PASS] " << label << "\n";                             \
        ++s_passed;                                                             \
    } else {                                                                    \
        std::cout << "  [FAIL] " << label                                      \
                  << " | expected " << (expected)                              \
                  << " got " << (actual) << "\n";                              \
        ++s_failed;                                                             \
    }

void PrintSummary() {
    std::cout << "\n==============================\n";
    std::cout << "  Passed: " << s_passed << "\n";
    std::cout << "  Failed: " << s_failed << "\n";
    std::cout << "==============================\n";
}

// ─────────────────────────────────────────────
// Test declarations
// ─────────────────────────────────────────────

void TestComponentID();
void TestComponentColumn();
void TestArchetype();
void TestWorld();

// ─────────────────────────────────────────────

int main() {
    std::cout << "\n=== ComponentID ===\n";
    TestComponentID();

    std::cout << "\n=== ComponentColumn ===\n";
    TestComponentColumn();

    std::cout << "\n=== Archetype ===\n";
    TestArchetype();

    std::cout << "\n=== World ===\n";
    TestWorld();

    PrintSummary();
    return s_failed > 0 ? 1 : 0; // non-zero exit code on failure
}

// ─────────────────────────────────────────────
// ComponentID
// ─────────────────────────────────────────────

void TestComponentID() {
    uint32_t tID  = ecs::ComponentID::get<Transform>();
    uint32_t vID  = ecs::ComponentID::get<Velocity>();
    uint32_t tID2 = ecs::ComponentID::get<Transform>(); // must be same as tID

    EXPECT_EQ(tID, tID2,   "Transform ID is stable across calls")
    EXPECT_EQ(tID == vID, false, "Transform and Velocity have different IDs")
}

// ─────────────────────────────────────────────
// ComponentColumn
// ─────────────────────────────────────────────

void TestComponentColumn() {
    auto col = ecs::ComponentColumn::Create<Transform>();

    // push 5 elements — growth triggers on 5th (capacity 4 -> 8)
    col.PushBack(Transform{1, 2});
    col.PushBack(Transform{3, 4});
    col.PushBack(Transform{5, 6});
    col.PushBack(Transform{7, 8});
    col.PushBack(Transform{9, 10});

    auto& t0 = *static_cast<Transform*>(col.GetRow(0));
    auto& t4 = *static_cast<Transform*>(col.GetRow(4));

    EXPECT_EQ(t0.x, 1.0f, "Column row 0 x == 1")
    EXPECT_EQ(t0.y, 2.0f, "Column row 0 y == 2")
    EXPECT_EQ(t4.x, 9.0f, "Column row 4 x == 9 (after grow)")

    // modify in place
    t0.x = 99.0f;
    auto& t0again = *static_cast<Transform*>(col.GetRow(0));
    EXPECT_EQ(t0again.x, 99.0f, "Column row 0 modification persists")

    // swap-and-pop row 0 — row 4 (last) should move into row 0
    col.SwapAndPop(0);
    auto& swapped = *static_cast<Transform*>(col.GetRow(0));
    EXPECT_EQ(swapped.x, 9.0f,  "After SwapAndPop(0), row 0 x == 9 (was last)")
    EXPECT_EQ(swapped.y, 10.0f, "After SwapAndPop(0), row 0 y == 10 (was last)")
}

// ─────────────────────────────────────────────
// Archetype
// ─────────────────────────────────────────────

void TestArchetype() {
    ecs::Signature sig = {
        ecs::ComponentID::get<Transform>(),
        ecs::ComponentID::get<Velocity>()
    };
    std::sort(sig.begin(), sig.end());

    ecs::Archetype arch(sig);
    arch.AddColumn<Transform>();
    arch.AddColumn<Velocity>();

    arch.PushBack(0u, Transform{1, 2}, Velocity{3, 4});
    arch.PushBack(1u, Transform{5, 6}, Velocity{7, 8});
    arch.PushBack(2u, Transform{9, 10}, Velocity{11, 12});

    EXPECT_EQ(arch.GetRowCount(), 3u, "Archetype row count == 3 after 3 pushes")

    // read
    EXPECT_EQ(arch.GetComponent<Transform>(0).x, 1.0f,  "Archetype row 0 Transform.x == 1")
    EXPECT_EQ(arch.GetComponent<Velocity>(1).vx, 7.0f,  "Archetype row 1 Velocity.vx == 7")
    EXPECT_EQ(arch.GetComponent<Transform>(2).x, 9.0f,  "Archetype row 2 Transform.x == 9")

    // modify
    arch.GetComponent<Transform>(0).x = 99.0f;
    EXPECT_EQ(arch.GetComponent<Transform>(0).x, 99.0f, "Archetype in-place modification persists")

    // remove row 0 — row 2 (last) should swap into row 0
    arch.RemoveRow(0);
    EXPECT_EQ(arch.GetRowCount(), 2u,   "Row count == 2 after RemoveRow")
    EXPECT_EQ(arch.GetComponent<Transform>(0).x, 9.0f,  "After RemoveRow(0), row 0 == old row 2")

    // HasComponent
    EXPECT_EQ(arch.HasComponent(ecs::ComponentID::get<Transform>()), true,  "Archetype HasComponent Transform")
    EXPECT_EQ(arch.HasComponent(ecs::ComponentID::get<Velocity>()),  true,  "Archetype HasComponent Velocity")
    EXPECT_EQ(arch.HasComponent(999u), false, "Archetype does not have unknown component")
}

// ─────────────────────────────────────────────
// World
// ─────────────────────────────────────────────

void TestWorld() {
    ecs::World world;

    auto e1 = world.CreateEntity();
    auto e2 = world.CreateEntity();
    auto e3 = world.CreateEntity();

    world.AddComponents(e1, Transform{1,  2},  Velocity{3,  4});
    world.AddComponents(e2, Transform{5,  6},  Velocity{7,  8});
    world.AddComponents(e3, Transform{9,  10}, Velocity{11, 12});

    // read
    EXPECT_EQ(world.GetComponent<Transform>(e1).x, 1.0f, "World e1 Transform.x == 1")
    EXPECT_EQ(world.GetComponent<Velocity>(e2).vx, 7.0f, "World e2 Velocity.vx == 7")
    EXPECT_EQ(world.GetComponent<Transform>(e3).x, 9.0f, "World e3 Transform.x == 9")

    // modify
    world.GetComponent<Transform>(e1).x = 99.0f;
    EXPECT_EQ(world.GetComponent<Transform>(e1).x, 99.0f, "World e1 Transform modified")

    // destroy e2 — e3 should swap into e2's old slot
    world.DestroyEntity(e2);
    EXPECT_EQ(world.GetComponent<Transform>(e3).x, 9.0f,  "e3 still accessible after e2 destroyed")
    EXPECT_EQ(world.GetComponent<Velocity>(e3).vx, 11.0f, "e3 Velocity intact after swap")

    // e1 still intact
    EXPECT_EQ(world.GetComponent<Transform>(e1).x, 99.0f, "e1 unaffected by e2 destruction")

    // cleanup
    world.DestroyEntity(e1);
    world.DestroyEntity(e3);

    // fresh world — entity IDs restart
    ecs::World world2;
    auto a = world2.CreateEntity();
    auto b = world2.CreateEntity();
    world2.AddComponents(a, Transform{0, 0}, Velocity{0, 0});
    world2.AddComponents(b, Transform{1, 1}, Velocity{1, 1});
    world2.DestroyEntity(a);
    EXPECT_EQ(world2.GetComponent<Transform>(b).x, 1.0f, "Isolated world: b intact after a destroyed")
}