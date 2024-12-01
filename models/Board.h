#pragma once

#include <vector>
#include <iostream>
#include <array>
#include <ranges>

#include "Card.h"
#include "Explosion.h"

class Board {
    std::vector<std::vector<std::vector<Card>>> m_board{};
    std::pair<size_t, size_t> m_hole{ -1, -1 };

    std::pair<size_t, size_t> m_minus{ -1, -1 };
    std::pair<size_t, size_t> m_plus{ -1, -1 };

    size_t m_restrictedRow = -1;
    size_t m_restrictedCol = -1;

    bool m_justBlocked = false;

    friend class Game;
    friend class Wizard;
    friend class Power;

    explicit Board(size_t _size);
    [[nodiscard]] size_t getSize() const;

    void circularShiftUp();
    void circularShiftDown();
    void circularShiftLeft();
    void circularShiftRight();

    void printBoard() const;

    [[nodiscard]] bool isAPile(size_t _row, size_t _col) const;
    [[nodiscard]] bool checkIndexes(size_t _row, size_t _col) const;
    [[nodiscard]] bool checkNeighbours(size_t _row, size_t _col) const;
    [[nodiscard]] bool checkValue(size_t _row, size_t _col, Card::Value _value, bool _illusion = false) const;
    [[nodiscard]] bool checkHole(size_t _row, size_t _col) const;

    [[nodiscard]] bool checkIllusion(size_t _row, size_t _col, Card::Color _color) const;
    [[nodiscard]] bool checkIllusionValue(size_t _row, size_t _col, size_t _value) const;

    void resetIllusion(size_t _row, size_t _col);

    [[nodiscard]] Card::Color checkWin() const;

    [[nodiscard]] bool checkFullBoard() const;
    [[nodiscard]] Card::Color calculateWinner() const;

    [[nodiscard]] bool checkTwoRows() const;
    std::vector<Card> useExplosion(const std::vector<std::vector<Explosion::ExplosionEffect>>& _matrix);

    [[nodiscard]] bool checkBoardIntegrity() const;

    void placeCard(size_t _row, size_t _col, const Card&& _card);
};
