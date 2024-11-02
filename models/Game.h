#pragma once

#include <vector>
#include <memory>
#include <array>

class Game {
public:
    static Game& getInstance(const size_t& array_size = 3) {
        static Game instance(array_size);
        return instance;
    }

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

private:
    size_t m_array_size;
    std::vector<std::vector<std::shared_ptr<std::vector<int>>>> m_array;
    std::array<size_t, 4> m_powers_used{};
    std::array<std::shared_ptr<void>, 2> m_players;

    explicit Game(const size_t& array_size) : m_array_size{ array_size } {
        m_array.resize(array_size, std::vector<std::shared_ptr<std::vector<int>>>(array_size, nullptr));
        m_powers_used.fill(0);
        m_players.fill(nullptr);
    }
};