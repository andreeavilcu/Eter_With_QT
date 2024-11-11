#include "Player.h"

Player::Player(const Card::Color _color, const std::vector<Card>& _cards, const bool _wizard, const bool _powers) :
    m_color{ _color },
    m_cards{ _cards } {

    for (size_t i = 0; i < _cards.size(); ++i)
        m_cards[i].setColor(_color);

    std::random_device rd;
    std::mt19937 gen{ rd() };

    std::uniform_int_distribution<size_t> wizardDistribution{ 0, Wizard::wizard_count - 1 };
    std::uniform_int_distribution<size_t> powerDistribution{ 0, Power::power_count - 1 };

    m_wizard_index = _wizard ? wizardDistribution(gen) : -1;
    m_powers_index.first = _powers ? powerDistribution(gen) : -1;

    do {
        m_powers_index.second = _powers ? powerDistribution(gen) : -1;
    } while (m_powers_index.first == m_powers_index.second && _powers);
}

void Player::returnCard(const Card &_card) {
    this->m_cards.push_back(_card);
}

void Player::printCards() {
    for (const auto & m_card : m_cards)
        std::cout << m_card;
}

Card::Color Player::getColor() const {
    return this->m_color;
}

size_t Player::getCardCount() const {
    return this->m_cards.size();
}

size_t Player::getCardCount(Card::Value _value) const {
    size_t count = 0;

    for (const auto& card : m_cards)
        if (card.getValue() == _value)
            ++count;

    return count;
}

int Player::getWizardIndex() const {
    return static_cast<int>(this->m_wizard_index);
}

std::pair<int, int> Player::getPowersIndex() const {
    return this->m_powers_index;
}

bool Player::wasIllusionPlayed() const {
    return this->m_playedIllusion;
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

std::optional<Card> Player::useIllusion(const Card::Value _value) {
    if(m_playedIllusion)
        return std::nullopt;

    m_playedIllusion = true;

    auto playedCard = this->useCard(_value);
    playedCard->setIllusion();

    return playedCard;
}
