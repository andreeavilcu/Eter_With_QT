#include "Player.h"

const std::vector<Card>& Player::getCards() const
{
    return m_cards;
}

void Player::setCards(const std::vector<Card>& _cards)
{
    this->m_cards = _cards;
}

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
    m_powers_index.first = _powers ? /*powerDistribution(gen)*/ 16: -1;

    do {
        m_powers_index.second = _powers ? /*powerDistribution(gen)*/ 17: -1;
    } while (m_powers_index.first == m_powers_index.second && _powers);
}

void Player::returnCard(const Card &_card) {
    this->m_cards.push_back(_card);
}

void Player::printCards() {
    for (const auto & m_card : m_cards)
        std::cout << m_card;

    std::cout << std::endl;
}

void Player::placeCard(size_t row, size_t col) {
    m_last_placed_card = {row, col};
}

void Player::addEliminatedCard(const Card &_card) {
    m_eliminated_cards.push_back(_card);
}

std::vector<Card> Player::getEliminatedCards() {
    return m_eliminated_cards;
}

void Player::setEliminatedCards(const std::vector<Card>& _cards)
{
    this->m_eliminated_cards = _cards;
}

Card::Color Player::getColor() const {
    return this->m_color;
}

void Player::setColor(Card::Color _color)
{
    this->m_color = _color;
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

std::pair<size_t, size_t> Player::getLastPlacedCard() const {
    return m_last_placed_card;
}

void Player::setLastPlacedCard(std::pair<size_t, size_t> _cardPosition)
{
    this->m_last_placed_card = _cardPosition;
}

int Player::getWizardIndex() const {
    return static_cast<int>(this->m_wizard_index);
}

void Player::setWizardIndex(size_t _index)
{
    this->m_wizard_index = _index;
}

std::pair<int, int> Player::getPowersIndex() const {
    return this->m_powers_index;
}

void Player::setPowersIndex(std::pair<size_t, size_t> _index)
{
    this->m_powers_index = _index;
}

bool Player::getPlayedIllusion() const
{
    return m_playedIllusion;
}

void Player::setPlayedIllusion(bool _played)
{
    this->m_playedIllusion = _played;
}

bool Player::wasIllusionPlayed() const {
    return this->m_playedIllusion;
}

bool Player::useWizard(Game& _game) {
    if (m_wizard_index == -1)
        return false;

    const bool legal = Wizard::getInstance().play(m_wizard_index, *this, _game);

    if (legal)
        m_wizard_index = -1;

    return legal;
}

bool Player::usePower(Game& _game, const bool _first) {
    if (_first ? m_powers_index.first == -1 : m_powers_index.second == -1)
        return false;

    const bool legal = Power::getInstance().play(_first ? m_powers_index.first : m_powers_index.second, *this, _game);

    if (legal)
        _first ? m_powers_index.first = -1 : m_powers_index.second = -1;

    return legal;
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
