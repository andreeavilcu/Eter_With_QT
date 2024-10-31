#include "Player.h"

Player::Player(const std::vector<PlayingCard> &playing_cards, const Wizard &wizard, const std::pair<Power, Power> &powers) {}

void Player::move() const{

}

std::vector<PlayingCard> Player::get_playing_cards() const {
    return m_playing_cards;
}

Wizard Player::get_wizard() const {
    return m_wizard;
}

std::pair<Power, Power> Player::get_powers() const {
    return m_powers;
}
