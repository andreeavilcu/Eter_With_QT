#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <ranges>
#include <array>
#include <list>

#include "Player.h"

template<size_t gridSize>
class Game {
private:
    Game() = default;
    ~Game() = default;

public:
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    static Game& getInstance() {
        static Game<gridSize> instance;
        return instance;
    }

private:
    template<size_t size>
    class Board {
    public:
        Board() = default;
        ~Board() = default;

    private:
        std::array<std::array<std::list<Card>, size>, size> m_board{};

        class RowProxy {
        private:
            std::array<std::list<Card>, size>& m_row;

        public:
            explicit RowProxy(std::array<std::list<Card>, size>& row) : m_row(row) {}

            std::list<Card>& operator[](size_t _col) const {
                return m_row[_col];
            }
        };

    public:
        RowProxy operator[](size_t _row) const {
            return RowProxy{m_board[_row]};
        }

        void swapRow(size_t _first, size_t _second);
        void swapCol(size_t _first, size_t _second);

        void placeCard(size_t _row, size_t _col, const Card& _card);

        [[nodiscard]] bool checkNeighbours(size_t _row, size_t _col) const;
        [[nodiscard]] bool checkValue(size_t _row, size_t _col, const Card::Value& _value) const;

        void printBoard() const;
    };

    Board<gridSize> m_board;

public:
    void run();

    bool placeCard(uint8_t _iterationIndex);
};

template<size_t gridSize>
template<size_t size>
void Game<gridSize>::Board<size>::swapRow(const size_t _first, const size_t _second) {
    if (_first < size && _second < size) {
        std::ranges::swap(m_board[_first], m_board[_second]);
    }
}

template<size_t gridSize>
template<size_t size>
void Game<gridSize>::Board<size>::swapCol(const size_t _first, const size_t _second) {
    if (_first < size && _second < size) {
        for (size_t i = 0; i < size; ++i) {
            std::ranges::swap(m_board[i][_first], m_board[i][_second]);
        }
    }
}

template<size_t gridSize>
template<size_t size>
void Game<gridSize>::Board<size>::placeCard(const size_t _row, const size_t _col, const Card& _card) {
    m_board[_row][_col].push_back(_card);
}

template<size_t gridSize>
template<size_t size>
bool Game<gridSize>::Board<size>::checkNeighbours(size_t _row, size_t _col) const {
    if (_row >= size || _col >= size) return false;

    const std::array<std::pair<int, int>, 8> directions = {{
        {-1, 0}, {1, 0}, {0, -1}, {0, 1},
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
    }};

    return std::ranges::any_of(directions, [&](const auto& direction) {
        int newRow = static_cast<int>(_row) + direction.first;
        int newCol = static_cast<int>(_col) + direction.second;

        return newRow >= 0 && newRow < static_cast<int>(size) &&
               newCol >= 0 && newCol < static_cast<int>(size) &&
               !m_board[newRow][newCol].empty();
    });
}

template<size_t gridSize>
template<size_t size>
bool Game<gridSize>::Board<size>::checkValue(size_t _row, size_t _col, const Card::Value& _value) const {
    if (m_board[_row][_col].back().getValue() < _value)
        return true;

    return false;
}

template<size_t gridSize>
template<size_t size>
void Game<gridSize>::Board<size>::printBoard() const {
    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            if (!m_board[i][j].empty()) {
                std::cout << m_board[i][j].back() << " ";
            } else {
                std::cout << 'x' << " ";
            }
        }
        std::cout << std::endl;
    }
}

template<size_t gridSize>
void Game<gridSize>::run() {
    std::random_device rd;
    std::mt19937 gen{ rd() };

    std::uniform_int_distribution<size_t> wizardDistribution{ 0, Wizard::wizard_count };
    std::uniform_int_distribution<size_t> powerDistribution{ 0, Power::power_count };

    Player p1{ {}, false, false };
    Player p2{ {}, false, false };
}

template<size_t gridSize>
bool Game<gridSize>::placeCard(uint8_t _iterationIndex) {

}
