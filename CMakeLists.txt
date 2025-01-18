cmake_minimum_required(VERSION 3.29)
project(Eter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nlohmann_json 3.2.0 REQUIRED)

add_executable(Eter source.cpp
        models/Actions/Explosion.h
        models/Actions/Explosion.cpp
        models/Actions/Wizard.h
        models/Actions/Wizard.cpp
        models/Actions/Power.h
        models/Actions/Power.cpp
        models/Actions/Card.h
        models/Actions/Card.cpp
        models/Actions/Piece.h
        models/Actions/Piece.cpp
        models/Player/Player.h
        models/Player/Player.cpp
        models/GameMechanics/GameEndInfo.h
        models/GameMechanics/Matrix.h
        models/GameMechanics/Board.h
        models/GameMechanics/Board.cpp
        models/GameMechanics/Game.h
        models/GameMechanics/Game.cpp
        models/GameMechanics/Match.h
        models/GameMechanics/Match.cpp
)

target_link_libraries(Eter PRIVATE nlohmann_json::nlohmann_json)