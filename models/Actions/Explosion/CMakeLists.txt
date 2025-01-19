cmake_minimum_required(VERSION 3.15)
project(ExplosionLib VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nlohmann_json REQUIRED)

add_library(explosion SHARED
        Explosion.cpp
        Explosion.h
)

set_target_properties(explosion PROPERTIES
        OUTPUT_NAME "explosion"
        VERSION 1.0
        SOVERSION 1
        PUBLIC_HEADER "Explosion.h")

target_include_directories(explosion PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(explosion PRIVATE nlohmann_json::nlohmann_json)
