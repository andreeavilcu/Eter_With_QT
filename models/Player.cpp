#include "Player.h"

Player::Player(const std::vector<Card>& _cards) :
    m_cards{ _cards } {
}

Player::Player(const std::vector<Card>& _cards, const size_t _wizard_index) :
    m_cards{ _cards },
    m_wizard_index{ _wizard_index } {
}

Player::Player(const std::vector<Card>& _cards, std::pair<size_t, size_t> _powers_index) :
    m_cards{ _cards },
    m_powers_index{ std::move(_powers_index) } {
}

Player::Player(const std::vector<Card>& _cards, const size_t _wizard_index, std::pair<size_t, size_t> _powers_index)
    : m_cards{ _cards }, m_wizard_index{ _wizard_index }, m_powers_index{ std::move(_powers_index) } {}

void Player::playWizard() const {
    Wizard::getInstance().play(m_wizard_index);
}

void Player::playPower(bool _first) const {
    Power::getInstance().play(_first ? m_powers_index.first : m_powers_index.second);
}

