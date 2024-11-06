#pragma once

#include <optional>
#include <utility>
#include <vector>
#include <random>

#include "Power.h"
#include "Card.h"
#include "Wizard.h"

class Player {
protected:
    Card::Color m_color{};

    std::vector<Card> m_cards{};

    size_t m_wizard_index{};
    std::pair<size_t, size_t> m_powers_index{};

public:
    explicit Player(Card::Color _color, const std::vector<Card>& _cards, bool _wizard, bool _powers);

    [[nodiscard]] Card::Color getColor() const;
    [[nodiscard]] size_t getCardCount() const;

    bool useWizard();
    bool usePower(bool _first);
    std::optional<Card> useCard(Card::Value _value);
};
