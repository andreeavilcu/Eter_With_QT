#pragma once

#include <vector>
#include <iostream>
#include <array>
#include <ranges>
#include <stack>
#include "Card.h"
#include "Explosion.h"
#include "Power.h"

class Board {
    std::vector<std::vector<std::vector<Card>>> m_board{};
public:

    friend class Game;
    friend class Wizard;
    friend class Power;

    explicit Board(size_t _size);
    [[nodiscard]] size_t getSize() const;
    [[nodiscard]] std::vector<std::vector<std::vector<Card>>>& getBoard();

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

    [[nodiscard]] Card::Color checkWin() const;

    [[nodiscard]] bool checkFullBoard() const;
    [[nodiscard]] Card::Color calculateWinner() const;

    [[nodiscard]] bool checkTwoRows() const;
    /*std::vector<Card>*/ void useExplosion(std::vector<Card>& returnedCards, std::vector<Card>& eliminatedCards);

    [[nodiscard]] bool checkBoardIntegrity() const;

    void placeCard(size_t _row, size_t _col, const Card&& _card);
};
