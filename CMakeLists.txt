cmake_minimum_required(VERSION 3.29)
project(Eter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Eter main.cpp
        utils/constants.h
        modules/Card.h
        modules/wizards_powers/Wizards.h
        modules/wizards_powers/Wizards.cpp
        modules/Wizard.h
        modules/Wizard.cpp)
