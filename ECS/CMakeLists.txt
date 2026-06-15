add_library(ECSLib STATIC
        src/Archetype.cpp
        src/World.cpp
        src/ComponentID.cpp
)

target_include_directories(ECSLib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/Components
#        TODO: NEED TO REMOVE COMPONENTS FROM HERE
)