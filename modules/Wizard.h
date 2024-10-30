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

void func1();

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
    func1, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
};

inline void func1() {
    std::cout << "func1" << std::endl;
}

#endif //WIZARD_H
