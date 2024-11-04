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
    std::array<std::array<std::shared_ptr<std::list<Card>>,gridSize>, gridSize> m_array;
    std::array<uint8_t, gridSize> colIndex, rowIndex;
    std::array<size_t, 4> m_powers_used{};
    std::array<std::shared_ptr<Player>, 2> m_players;

    explicit Game(const size_t& array_size) : m_array_size{ array_size } {
        m_array.resize(array_size, std::vector<std::shared_ptr<std::vector<int>>>(array_size, nullptr));
        m_powers_used.fill(0);
        m_players.fill(nullptr);
    }

public:
    void run();
    void explosion();
    void generateExplosion();
    void shiftMap();


};