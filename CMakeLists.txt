cmake_minimum_required(VERSION 3.20)
project(ECS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



add_subdirectory(ECS)
add_subdirectory(Sandbox)

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR NOT CMAKE_BUILD_TYPE)
    add_compile_options(-fsanitize=address -g)
    add_link_options(-fsanitize=address)
endif()