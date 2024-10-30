#pragma once

#include <functional>
#include <array>

#include "Card.h"
#include "wizards_powers/Wizards.h"

class Wizard : public Card {
public:
    using FuncType = std::function<void()>;

protected:
    static std::array<FuncType, eter::wizard_count> m_wizards;

public:
    explicit Wizard(const size_t _value);

    void playWizard() const;
};