#pragma once

#include <utility>
#include <vector>
#include <random>

#include "Power.h"
#include "Card.h"
#include "Wizard.h"

class Player {
protected:
    std::vector<Card> m_cards{};
    size_t m_wizard_index{};
    std::pair<size_t, size_t> m_powers_index{};

public:
    explicit Player(const std::vector<Card>&& _cards, bool _wizard, bool _powers);

    void useWizard();
    void usePower(bool _first);
};
