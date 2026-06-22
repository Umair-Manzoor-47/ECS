# ECS — Entity Component System

> A handwritten, archetype-based Entity Component System built in **C++20**.
>
> Developed from scratch as a standalone library for integration into my personal game engine, **OTTERUS**.

---

# What is an ECS?

An **Entity Component System (ECS)** is an alternative to traditional object-oriented game architecture.

Instead of a `Player` class that owns its own position, health, sprite, and behavior, an ECS separates those responsibilities into independent parts:

### Entity

A lightweight unique identifier.

* Contains no data
* Contains no behavior

### Component

A plain data structure.

Examples:

```cpp
Transform
Velocity
Health
Sprite
```

### System

Logic that operates on entities possessing a specific set of components.

Examples:

```cpp
MovementSystem
RenderSystem
PhysicsSystem
```

This separation allows:

* Flexible composition of behavior
* High-performance iteration over large numbers of entities
* Elimination of deep inheritance hierarchies
* Better cache locality

---

# Archetype-Based ECS Design

There are multiple ways to implement an ECS.

This project uses the **archetype model**, the same fundamental approach used by **Unity DOTS**.

## Core Idea

Every entity belongs to exactly one **archetype**, defined by the complete set of components attached to that entity.

```text
Archetype A: [Transform, Velocity]       ← entities 0, 1, 2
Archetype B: [Transform, Sprite]         ← entities 3, 4
Archetype C: [Transform, Velocity, HP]   ← entity 5
```

Each archetype stores component data in **packed parallel arrays**, one array per component type.

```text
Archetype A

Transform[] = [ T0, T1, T2 ]
Velocity[]  = [ V0, V1, V2 ]
               ↑   ↑   ↑
              e0  e1  e2
```

Iterating over all entities containing:

```cpp
Transform + Velocity
```

becomes a simple walk through contiguous memory.

### Benefits

* No pointer chasing
* Dense memory layout
* Cache-friendly iteration
* Better scalability for large entity counts

---

# Architecture

```text
ECS/
├── ComponentID
│   └── Gives every component type a unique integer at compile time
│
├── ComponentColumn
│   └── Type-erased, growable packed array for a single component type
│
├── Archetype
│   └── Owns ComponentColumns for a specific component signature
│
├── ArchetypeRegistry
│   └── Finds or creates archetypes for component signatures
│
├── EntityRegistry
│   └── Maps Entity → (Archetype, Row)
│
└── World
    └── Public API:
        • Create/Destroy entities
        • Add/Remove components
        • Execute queries
```

---

# Development Roadmap

## Core Infrastructure

* [x] `Entity` — lightweight ID wrapper
* [x] `ComponentID` — compile-time type → unique integer mapping
* [x] `ComponentColumn` — type-erased growable buffer with move semantics

## ECS Storage

* [x] `Archetype` — parallel column storage and row management
* [x] `ArchetypeRegistry` — signature-based archetype lookup and creation
* [x] `EntityRegistry` — entity → (archetype, row) mapping

## Public API

* [x] `World` — user-facing ECS interface
* [ ] Basic query system — iterate entities matching component signatures
* [ ] Integration into the OTTERUS game engine


---

# Dependencies

| Requirement           | Version |
| --------------------- | ------- |
| C++                   | 20      |
| CMake                 | 3.20+   |
| Third-Party Libraries | None    |

---


### References

The primary reference implementation is:

* EnTT — a production-grade ECS library

For now, the focus is on studying its architecture and documentation rather than diving directly into the source code.

Repository:

https://github.com/skypjack/entt
