//
// Created by Andrei PRIBOI on 30.10.2024.
//

#ifndef WIZARD_H
#define WIZARD_H

#include <functional>
#include <iostream>
#include <stdio.h>
#include <array>

#include "Card.h"

#include "../utils/constants.h"

void eliminateCard();
void eliminateRow();
void coverCard();
void sinkHole();
void moveStackOwn();
void extraEter();
void moveStackOpponent();
void moveEdge();

class Wizard : public Card {
public:
    using FuncType = std::function<void()>;

protected:
    static std::array<FuncType, eter::wizard_count> m_wizards;

public:
    explicit Wizard(const size_t _value) : Card(_value) {}

    void playWizard() const {
        m_wizards[this->m_value]();
    }
};

std::array<Wizard::FuncType, eter::wizard_count> Wizard::m_wizards = {
    eliminateCard, eliminateRow, coverCard, sinkHole, moveStackOwn, extraEter, moveStackOpponent, moveEdge
};

inline void eliminateCard() {
    std::cout << "eliminateCard" << std::endl;
}

inline void eliminateRow() {
    std::cout << "eliminateRow" << std::endl;
}

inline void coverCard() {
    std::cout << "coverCard" << std::endl;
}

inline void sinkHole() {
    std::cout << "sinkHole" << std::endl;
}

inline void moveStackOwn() {
    std::cout << "moveStackOwn" << std::endl;
}

inline void extraEter() {
    std::cout << "extraEter" << std::endl;
}

inline void moveStackOpponent() {
    std::cout << "moveStackOpponent" << std::endl;
}

inline void moveEdge() {
    std::cout << "moveEdge" << std::endl;
}

#endif //WIZARD_H
