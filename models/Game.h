#pragma once

#include<iostream>
#include <vector>
#include <memory>
#include <array>
#include <list>

#include "Player.h"

template<size_t gridSize>
class Game {
public:
    static Game& getInstance(const size_t& array_size = 3) {
        static Game instance(array_size);
        return instance;
    }

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

private:

public:
    void run();
    void explosion();
    void generateExplosion();
    void shiftMap();


};