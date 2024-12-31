#pragma once

#include <unordered_map>
#include <iostream>
#include <vector>
#include <ranges>
#include <stack>
#include <array>

#include "Matrix.h"
#include "../Actions/Card.h"
#include "../Actions/Explosion.h"
#include "../Actions/Power.h"
#include "../Actions/Wizard.h"

class Board {
    Matrix<Card> m_board{};
    bool m_firstCardPlayed = false;

public:
    friend class Game;
    friend class Wizard;
    friend class Power;

    std::pair<size_t, size_t> findCardIndexes(const Card* _card) const;

    void setFirstCardPlayed() { m_firstCardPlayed = true; }

    explicit Board(size_t _size);
    [[nodiscard]] size_t getSize() const;
    [[nodiscard]] std::vector<std::vector<std::vector<Card>>>& getBoard();

    bool circularShiftUp(bool _check = false);
    bool circularShiftDown(bool _check = false);
    bool circularShiftLeft(bool _check = false);
    bool circularShiftRight(bool _check = false);

    bool checkIfCanShift();

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
    void useExplosion(std::vector<Card>& returnedCards, std::vector<Card>& eliminatedCards);

    [[nodiscard]] bool checkBoardIntegrity() const;

    [[nodiscard]] bool checkPartial(size_t _x, size_t _y, size_t _value) const;

    std::optional<Card> placeCard(size_t _row, size_t _col, const Card&& _card);
};
