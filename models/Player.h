#pragma once
#include <vector>

#include "Card.h"
#include "Power.h"
#include "Wizard.h"

class Player {
protected:
    std::vector<Card> m_cards{};
    size_t m_wizard_index{};
    std::pair<size_t, size_t> m_powers_index{};

public:
    explicit Player(const std::vector<Card>& _cards);
    explicit Player(const std::vector<Card>& _cards, size_t _wizard_index);

    void playWizard() const; // TODO playPower si playCard
};
