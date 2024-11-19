cmake_minimum_required(VERSION 3.29)
project(Eter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Eter main.cpp
        models/Wizard.h
        models/Wizard.cpp
        models/Power.h
        models/Power.cpp
        models/Card.h
        models/Card.cpp
        models/Player.h
        models/Player.cpp
        models/Game.h
        models/Game.cpp
)
