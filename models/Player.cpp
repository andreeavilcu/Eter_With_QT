#include "Player.h"

#include "Game.h"

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

bool Player::useWizard(Game& _game, const bool _check) {
    if (m_wizard_index == -1)
        return false;

    const bool legal = Wizard::getInstance().play(m_wizard_index, *this, _game, _check);

    if (legal)
        m_wizard_index = -1;

    return legal;
}

bool Player::usePower(Game& _game, const bool _first, const bool _check) {
    if (_first ? m_powers_index.first == -1 : m_powers_index.second == -1)
        return false;

    const bool legal = Power::getInstance().play(_first ? m_powers_index.first : m_powers_index.second, *this, _game, _check);

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

void Player::shiftBoard(Game& _game) {
    char choice;

    std::cout << "enter direction (wasd)";
    std::cin >> choice;

    switch (choice) {
        case 'w':
            _game.getBoard().circularShiftUp();
        break;
        case 'a':
            _game.getBoard().circularShiftLeft();
        break;
        case 's':
            _game.getBoard().circularShiftDown();
        break;
        case 'd':
            _game.getBoard().circularShiftRight();
        break;
        default:
            break;
    }
}

bool Player::playCard(Game &_game) {
    size_t x, y, int_value;

    std::cin >> x;
    std::cin >> y;
    std::cin >> int_value;

    auto playedCard = this->playCardCheck(_game, x, y, int_value);

    if (!playedCard)
        return false;

    if (!_game.getBoard().checkIllusion(x, y, Card::Color::Undefined) && _game.getBoard().checkIllusionValue(x, y, int_value)) {
        _game.getBoard().placeCard(x, y, std::move(*playedCard));
        this->placeCard(x, y);
    }

    else {
        _game.getBoard().resetIllusion(x, y);
        _game.m_eliminatedCards.push_back(std::move(*playedCard));
    }

    return true;
}

std::optional<Card> Player::playCardCheck(Game &_game, const size_t _x, const size_t _y, const size_t _int_value) {
    if (!_game.checkPartial(_x, _y, _int_value))
        return std::nullopt;

    if (_game.getBoard().checkIllusion(_x, _y, this->m_color))
        return std::nullopt;

    auto playedCard = this->useCard(static_cast<Card::Value>(_int_value));

    return playedCard;
}

bool Player::playIllusion(Game &_game) {
    size_t x, y, int_value;

    std::cin >> x;
    std::cin >> y;
    std::cin >> int_value;

    auto playedCard = this->playCardCheck(_game, x, y, int_value);

    if (!playedCard)
        return false;

    _game.getBoard().placeCard(x, y, std::move(*playedCard));

    this->placeCard(x, y);

    return true;
}

std::optional<Card> Player::playIllusionCheck(Game &_game, const size_t _x, const size_t _y, const size_t _int_value) {
    if (!_game.checkPartial(_x, _y, _int_value))
        return std::nullopt;

    if (_game.getBoard().checkIllusion(_x, _y, Card::Color::Player1) || _game.getBoard().checkIllusion(_x, _y, Card::Color::Player2))
        return std::nullopt;

    auto playedCard = this->useCard(static_cast<Card::Value>(_int_value));

    return playedCard;
}

bool Player::playWizard(Game& _game, const bool _check = false) {
    return this->useWizard(_game, _check);
}

bool Player::playPower(Game &_game, const bool _check = false) {
    char choice;
    bool first;

    std::cout << "Press 'f' for first power and 's' for second power.\n";
    std::cin >> choice;

    if (tolower(choice) == 'f')
        first = true;

    else if (tolower(choice) == 's')
        first = false;

    else return false;

    return this->usePower(_game, first, _check);
}

void Player::playExplosion(Game& _game) {
    std::cout << "Player 2's turn\n";
    std::cout << "---------------\n";

    _game.getBoard().printBoard();

    bool quit = false;

    do {
        Explosion::getInstance().printExplosion();

        std::cout << "Press 'r' to rotate explosion or 'c' to confirm.\n";
        std::cout << "Press 'x' to to quit using explosion.\n";
    }
    while (!quit && Explosion::getInstance().rotateExplosion(quit));

    _game.m_playedExplosion = true;

    if (quit)
        return;

    _game.m_returnedCards = _game.getBoard().useExplosion();

    for (auto card : _game.m_returnedCards) {
        card.getColor() == Card::Color::Player1
            ? _game.getPlayer1().returnCard(card)
            : _game.getPlayer2().returnCard(card);
    }
}

bool Player::playerTurn(Game &_game) {
    char choice;
    bool legal = false;

    std::cout << "Play a card   (c) ";
    this->printCards();

    std::cout << "Shift board   (s)\n";

    if (_game.getGameType() == Game::GameType::WizardDuel || _game.getGameType() == Game::GameType::WizardAndPowerDuel) {
        std::cout << "Play wizard   (w) " << this->getWizardIndex() << "\n";
    }

    if (_game.getGameType() == Game::GameType::PowerDuel || _game.getGameType() == Game::GameType::WizardAndPowerDuel) {
        std::cout << "Play power    (p) " <<
        std::to_string(this->getPowersIndex().first) + " " + std::to_string(this->getPowersIndex().second)
         << "\n";
    }

    std::cout << "Play illusion (i) " << (
         this->wasIllusionPlayed() ? "(already played)" : ""
    ) << "\n";
    std::cin >> choice;

    switch (choice) {
        case 'c':
            legal = this->playCard(_game);
            break;

        case 's':
            this->shiftBoard(_game);
            legal = false;
            break;

        case 'i':
            legal = this->playIllusion(_game);
            break;

        case 'w':
            if (_game.getGameType() == Game::GameType::WizardDuel || _game.getGameType() == Game::GameType::WizardAndPowerDuel)
                legal = this->playWizard(_game);

            break;

        case 'p':
            if (_game.getGameType() == Game::GameType::PowerDuel || _game.getGameType() == Game::GameType::WizardAndPowerDuel)
                legal = this->playPower(_game);

            break;

        default:
            break;
    }

    if (!legal)
        return false;

    if (!_game.m_playedExplosion && _game.getBoard().checkTwoRows())
        this->playExplosion(_game);

    return true;
}
