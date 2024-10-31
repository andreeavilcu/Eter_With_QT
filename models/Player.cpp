#include "Player.h"

Player::Player(const std::vector<Card> &_cards) :
    m_cards(_cards) {
}

Player::Player(const std::vector<Card> &_cards, const size_t _wizard_index) :
    m_cards(_cards),
    m_wizard_index(_wizard_index) {
}

void Player::playWizard() const {
    Wizard::getInstance().play(m_wizard_index);
}
