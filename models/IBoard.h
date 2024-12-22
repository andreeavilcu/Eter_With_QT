#pragma once

#include <unordered_map>
#include <iostream>
#include <vector>
#include <ranges>
#include <stack>
#include <array>

#include "Card.h"
#include "Explosion.h"
#include "Power.h"
#include "Wizard.h"

template<typename T>
class IBoard {
public:
    virtual ~IBoard() = default;

    virtual size_t getSize() const = 0;
    virtual std::vector<std::vector<std::vector<T>>>& getBoard() = 0;

    virtual void circularShiftUp() = 0;
    virtual void circularShiftDown() = 0;
    virtual void circularShiftLeft() = 0;
    virtual void circularShiftRight() = 0;

    virtual void printBoard() const = 0;


    virtual T::Color checkWin() const = 0;
    virtual T::Color calculateWinner() const = 0;

    virtual void useExplosion(std::vector<T>& returnedCards, std::vector<T>& eliminatedCards) = 0;

    virtual std::optional<T> placeCard(size_t row, size_t col, const T&& card) = 0;

};