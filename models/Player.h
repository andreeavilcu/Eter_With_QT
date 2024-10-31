#pragma once
#include <vector>

#include "cards/Wizard.h"
#include "cards/PlayingCard.h"
#include "cards/Power.h"

class Player {
protected:
    std::vector<PlayingCard> m_playing_cards;
    Wizard m_wizard;
    std::pair<Power, Power> m_powers;
public:
    explicit Player(const std::vector<PlayingCard>& playing_cards, const Wizard& wizard, const std::pair<Power, Power>& powers);
    void move() const;
};
