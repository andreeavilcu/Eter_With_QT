#include "Player.h"

Player::Player(const std::vector<Card>&& _cards, const bool _wizard, const bool _powers) :
    m_cards{ _cards } {

    std::random_device rd;
    std::mt19937 gen{ rd() };

    std::uniform_int_distribution<size_t> wizardDistribution{ 0, Wizard::wizard_count };
    std::uniform_int_distribution<size_t> powerDistribution{ 0, Power::power_count };

    m_wizard_index = _wizard ? wizardDistribution(gen) : -1;
    m_powers_index.first = _powers ? powerDistribution(gen) : -1;

    do {
        m_powers_index.second = _powers ? powerDistribution(gen) : -1;
    } while (m_powers_index.first == m_powers_index.second && _powers);
}

void Player::useWizard() {
    if (m_wizard_index == -1)
        return;

    Wizard::getInstance().play(m_wizard_index);
    m_wizard_index = -1;
}

void Player::usePower(bool _first) {
    if (_first ? m_powers_index.first == -1 : m_powers_index.second == -1)
        return;

    Power::getInstance().play(_first ? m_powers_index.first : m_powers_index.second);
    _first ? m_powers_index.first = -1 : m_powers_index.second = -1;
}