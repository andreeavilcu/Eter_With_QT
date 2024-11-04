#include "Player.h"

Player::Player(const Card::Color _color, const std::vector<Card>& _cards, const bool _wizard, const bool _powers) :
    m_color{ _color },
    m_cards{ _cards } {

    for (size_t i = 0; i < _cards.size(); ++i)
        m_cards[i].setColor(_color);

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

bool Player::useWizard() {
    if (m_wizard_index == -1)
        return false;

    Wizard::getInstance().play(m_wizard_index);
    m_wizard_index = -1;

    return true;
}

bool Player::usePower(bool _first) {
    if (_first ? m_powers_index.first == -1 : m_powers_index.second == -1)
        return false;

    Power::getInstance().play(_first ? m_powers_index.first : m_powers_index.second);
    _first ? m_powers_index.first = -1 : m_powers_index.second = -1;

    return true;
}

std::optional<Card> Player::useCard(Card::Value _value) {
    const auto iter = std::ranges::find_if(m_cards, [_value](const Card& _card) {
        return _card.getValue() == _value;
    });

    if (iter != m_cards.end()) {
        Card cardToMove = std::move(*iter);
        m_cards.erase(iter);
        return cardToMove;
    }

    return std::nullopt;
}
