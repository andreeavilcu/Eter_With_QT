#pragma once
#include <vector>

#include "Card.h"
#include "Power.h"

class Player {
protected:
    // std::vector<Card> m_playing_cards;
    size_t m_wizard_index;
    std::pair<size_t, size_t> m_powers_index;
public:

    // explicit Player(const size_t& _value);
    // explicit Player(const std::vector<PlayingCard>& playing_cards, size_t _wizard_index, const std::pair<size_t, size_t>& _powers_index);
    void move() const;
    // std::vector<Card> get_playing_cards() const;
    // Wizard get_wizard() const;
    // std::pair<Power, Power> get_powers() const;
};
