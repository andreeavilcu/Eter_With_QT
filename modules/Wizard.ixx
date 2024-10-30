#include <functional>
#include <iostream>
#include <ostream>
#include <cstdint>

#include "../utils/constants.h"

export module Wizard;

import Card;

void func1();

export class Wizard : public Card {
public:
    using FuncType = std::function<void()>;

protected:
    static std::array<FuncType, eter::WIZARD_COUNT> m_wizards;

public:
    explicit Wizard(const uint8_t _value) : Card(_value) {
        Wizard::m_wizards.fill(nullptr);

        // testing purposes
        Wizard::m_wizards[0] = func1;
    }

    void playWizard() const {
        Wizard::m_wizards[this->m_value]();
    }
};

void func1() {
    std::cout << "func1" << std::endl;
}