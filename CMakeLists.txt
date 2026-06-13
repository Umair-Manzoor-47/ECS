cmake_minimum_required(VERSION 4.2)
project(ECS)

set(CMAKE_CXX_STANDARD 20)

add_executable(ECS Sandbox/main.cpp)

include_directories(
        ${PROJECT_SOURCE_DIR} include/
        ${PROJECT_SOURCE_DIR} src/
)