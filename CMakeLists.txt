cmake_minimum_required(VERSION 3.29)
project(Eter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Eter main.cpp
        utils/constants.h
        models/cards/Card.h
        models/cards/Card.cpp
        models/cards/Wizard.h
        models/cards/Wizard.cpp
        models/cards/PlayingCard.h
        models/cards/PlayingCard.cpp
        models/cards/Power.h
        models/cards/Power.cpp
        models/Player.h
        models/Player.cpp
        wizards_powers/Wizards.h
        wizards_powers/Wizards.cpp
        wizards_powers/Powers.h
        wizards_powers/Powers.cpp)
