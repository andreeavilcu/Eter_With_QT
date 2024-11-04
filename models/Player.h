#pragma once

#include <utility>
#include <vector>
#include <random>

#include "Power.h"
#include "Card.h"
#include "Wizard.h"

class Player {
public:
    enum class Color: uint8_t {
        Red = 0,
        Blue
    };

protected:
    Color m_color{};

    std::vector<Card> m_cards{};

    size_t m_wizard_index{};
    std::pair<size_t, size_t> m_powers_index{};

public:
    explicit Player(Color _color, const std::vector<Card>& _cards, bool _wizard, bool _powers);

    bool useWizard();
    bool usePower(bool _first);
    std::optional<Card> useCard(Card::Value _value);
};
