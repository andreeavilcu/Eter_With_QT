#include "Game.h"

Game::Game(const GameType _gameType) :
    m_board{ _gameType == Game::GameType::Training
       ? static_cast<size_t>(GridSize::Three)
       : static_cast<size_t>(GridSize::Four) },

    m_gameType{ _gameType },

    m_player1{ Card::Color::Player1, (_gameType == GameType::Training
        ? std::vector<Card>{
            Card{Card::Value::One}, Card{Card::Value::One},
            Card{Card::Value::Two}, Card{Card::Value::Two},
            Card{Card::Value::Three}, Card{Card::Value::Three},
            Card{Card::Value::Four}
        }
        : std::vector<Card>{
            Card{Card::Value::Eter}, Card{Card::Value::One},
            Card{Card::Value::One}, Card{Card::Value::Two},
            Card{Card::Value::Two}, Card{Card::Value::Two},
            Card{Card::Value::Three}, Card{Card::Value::Three},
            Card{Card::Value::Three}, Card{Card::Value::Four}
        }),
        _gameType == GameType::WizardDuel || _gameType == GameType::WizardAndPowerDuel,
        _gameType == GameType::PowerDuel || _gameType == GameType::WizardAndPowerDuel
    },

    m_player2{ Card::Color::Player2, (_gameType == GameType::Training
        ? std::vector<Card>{
            Card{Card::Value::One}, Card{Card::Value::One},
            Card{Card::Value::Two}, Card{Card::Value::Two},
            Card{Card::Value::Three}, Card{Card::Value::Three},
            Card{Card::Value::Four}
        }
        : std::vector<Card>{
            Card{Card::Value::Eter}, Card{Card::Value::One},
            Card{Card::Value::One}, Card{Card::Value::Two},
            Card{Card::Value::Two}, Card{Card::Value::Two},
            Card{Card::Value::Three}, Card{Card::Value::Three},
            Card{Card::Value::Three}, Card{Card::Value::Four}
        }),
        _gameType == GameType::WizardDuel || _gameType == GameType::WizardAndPowerDuel,
        _gameType == GameType::PowerDuel || _gameType == GameType::WizardAndPowerDuel
    } {}

void Game::run() {
    size_t iterationIndex = 0;

    Power& power = Power::getInstance();

    while (checkEndOfGame(!(iterationIndex % 2) ? Card::Color::Player1 : Card::Color::Player2)) {
        std::cout << "Player " << iterationIndex % 2 + 1 << "'s turn!" << std::endl;

        this->m_board.printBoard();

        if (playerTurn(iterationIndex % 2 ? Card::Color::Player2 : Card::Color::Player1, iterationIndex)) {
            iterationIndex++;

            if (power.getJustBlocked())
                power.setJustBlocked(false);

            else if (power.getRestrictedCol() == -1 || power.getRestrictedRow() == -1) {
                power.setRestrictedCol(-1);
                power.setRestrictedRow(-1);
            }
        }

        if (!this->m_playedExplosion && this->m_board.checkTwoRows())
            this->playExplosion();

        else if (!m_returnedCards.empty()) {
            for (auto& card : m_returnedCards) {
                auto returnedCard = std::move(card);

                if (returnedCard.getColor() == Card::Color::Player1)
                    m_player1.returnCard(std::move(returnedCard));

                else
                    m_player2.returnCard(std::move(returnedCard));
            }
            m_returnedCards.clear();
        }
    }

    this->m_board.printBoard();

    if (m_winner == Card::Color::Undefined) {
        std::cout << "Draw" << std::endl;
        return;
    }

    std::cout << "Winner: " << (m_winner == Card::Color::Player1 ? "Player 1" : "Player 2") << std::endl;
}
void Game::shiftBoard() {
    char choice;

    std::cin >> choice;

    switch (choice) {
        case 'w':
            this->m_board.circularShiftUp();
        break;
        case 'a':
            this->m_board.circularShiftLeft();
        break;
        case 's':
            this->m_board.circularShiftDown();
        break;
        case 'd':
            this->m_board.circularShiftRight();
        break;
        default:
            break;
    }
}

bool Game::checkEmptyDeck() const {
    return !m_player1.getCardCount() || !m_player2.getCardCount();
}

bool Game::checkCardAfterReturn(const Card::Color _color, const Card::Value _value) const {
    const auto iter = std::ranges::find_if(m_returnedCards, [_value](const Card& _card) {
        return _card.getValue() == _value;
    });

    if (iter == m_returnedCards.end())
        return true;

    if (_color == Card::Color::Player1)
        return m_player1.getCardCount(_value) > 1;

    return m_player2.getCardCount(_value) > 1;
}

bool Game::checkEndOfGame(const Card::Color _color) {
    this->m_winner = this->m_board.checkWin();

    if (this->m_winner != Card::Color::Undefined)
        return false;

    if (checkEmptyDeck() || this->m_board.checkFullBoard()) {
        std::cout << "Player " << (_color == Card::Color::Player1 ? 1 : 2) << "'s turn!" << std::endl;
        this->m_board.printBoard();

        playerTurn(_color, -1);

        this->m_winner = this->m_board.calculateWinner();

        return false;
    }

    return true;
}

bool Game::checkPartial(const size_t _x, const size_t _y, const size_t _int_value, const size_t _iterationIndex) const {
    if (!this->m_board.checkIndexes(_x, _y) || this->m_board.checkHole(_x, _y))
        return false;

    if (_int_value > static_cast<size_t>(Card::Value::Four))
        return false;

    const auto value = static_cast<Card::Value>(_int_value);

    if (!this->m_board.checkValue(_x, _y, value))
        return false;

    if (_iterationIndex && !this->m_board.checkNeighbours(_x, _y))
        return false;


    const auto& power = Power::getInstance();

    if (_x == power.getRestrictedRow() || _y == power.getRestrictedCol()) {
        return false;
    }
    return true;
}

bool Game::playCard(const Card::Color _color, const size_t _iterationIndex) {
    size_t x, y, int_value;

    std::cin >> x;
    std::cin >> y;
    std::cin >> int_value;

    if (!this->checkPartial(x, y, int_value, _iterationIndex))
        return false;

    if (this->m_board.checkIllusion(x, y, _color))
        return false;

    auto playedCard =
        m_player1.getColor() == _color ?
            m_player1.useCard(static_cast<Card::Value>(int_value)) :
            m_player2.useCard(static_cast<Card::Value>(int_value));

    if (!playedCard)
        return false;

    if (!this->m_board.checkIllusion(x, y, Card::Color::Undefined) && this->m_board.checkIllusionValue(x, y, int_value)) {
        this->m_board.placeCard(x, y, std::move(*playedCard));
        (m_player1.getColor() == _color ? m_player1 : m_player2).placeCard(x, y);
    }

    else {
        this->m_board.resetIllusion(x, y);
        playedCard.reset();
    }



    return true;
}

bool Game::playIllusion(const Card::Color _color, const size_t _iterationIndex) {
    size_t x, y, int_value;

    std::cin >> x;
    std::cin >> y;
    std::cin >> int_value;

    if (!this->checkPartial(x, y, int_value, _iterationIndex))
        return false;

    if (this->m_board.checkIllusion(x, y, Card::Color::Player1) || this->m_board.checkIllusion(x, y, Card::Color::Player2))
        return false;

    auto playedCard =
        _color == Card::Color::Player1 ?
            m_player1.useIllusion(static_cast<Card::Value>(int_value)) :
            m_player2.useIllusion(static_cast<Card::Value>(int_value));

    if (!playedCard)
        return false;

    this->m_board.placeCard(x, y, std::move(*playedCard));

    (m_player1.getColor() == _color ? m_player1 : m_player2).placeCard(x, y);

    return true;
}

bool Game::playWizard(const Card::Color _color) {
    if (_color == Card::Color::Player1)
        return m_player1.useWizard(*this);

    return m_player2.useWizard(*this);
}

bool Game::playPower(const Card::Color _color) {
    char choice;
    bool first;

    std::cout << "Press 'f' for first power and 's' for second power.\n";
    std::cin >> choice;

    if (tolower(choice) == 'f')
        first = true;

    else if (tolower(choice) == 's')
        first = false;

    else return false;

    if (_color == Card::Color::Player1)
        return m_player1.usePower(*this, first);

    return m_player2.usePower(*this, first);
}

void Game::playExplosion() {
    std::vector<std::vector<Explosion::ExplosionEffect>> explosionEffects = Explosion::getInstance().generateExplosion(this->m_board.getSize());

    std::cout << "Player 2's turn\n";
    std::cout << "---------------\n";

    this->m_board.printBoard();

    bool quit = false;

    do {
        Explosion::getInstance().printExplosion(explosionEffects);

        std::cout << "Press 'r' to rotate explosion or 'c' to confirm.\n";
        std::cout << "Press 'x' to to quit using explosion.\n";
    }
    while (!quit && Explosion::getInstance().rotateExplosion(explosionEffects, quit));

    m_playedExplosion = true;

    if (quit)
        return;

    this->m_returnedCards = this->m_board.useExplosion(explosionEffects);

    for (auto card : this->m_returnedCards) {
        card.getColor() == Card::Color::Player1
            ? m_player1.returnCard(card)
            : m_player2.returnCard(card);
    }
}

bool Game::playerTurn(const Card::Color _color, const size_t _iterationIndex) {
    char choice;

    std::cout << "Play a card   (c) ";
    _color == Card::Color::Player1
        ? m_player1.printCards()
        : m_player2.printCards();

    std::cout << "Shift board   (s)\n";

    if (m_gameType == GameType::WizardDuel || m_gameType == GameType::WizardAndPowerDuel) {
        std::cout << "Play wizard   (w) " << (
        _color == Card::Color::Player1
            ? m_player1.getWizardIndex()
            : m_player2.getWizardIndex()
        ) << "\n";
    }

    if (m_gameType == GameType::PowerDuel || m_gameType == GameType::WizardAndPowerDuel) {
        std::cout << "Play power    (p) " << (
            _color == Card::Color::Player1
                ? std::to_string(m_player1.getPowersIndex().first) + " " + std::to_string(m_player1.getPowersIndex().second)
                : std::to_string(m_player2.getPowersIndex().first) + " " + std::to_string(m_player2.getPowersIndex().second)
        ) << "\n";
    }

    std::cout << "Play illusion (i) " << (
        _color == Card::Color::Player1
            ? (m_player1.wasIllusionPlayed() ? "(already played)" : "")
            : (m_player2.wasIllusionPlayed() ? "(already played)" : "")
    ) << "\n";
    std::cin >> choice;

    switch (choice) {
        case 'c':
            return this->playCard(_color, _iterationIndex);

        case 's':
            this->shiftBoard();
            return false;

        case 'i':
            return this->playIllusion(_color, _iterationIndex);

        case 'w':
            if (m_gameType == GameType::WizardDuel || m_gameType == GameType::WizardAndPowerDuel)
                return this->playWizard(_color);

        case 'p':
            if (m_gameType == GameType::PowerDuel || m_gameType == GameType::WizardAndPowerDuel)
                return this->playPower(_color);
        default:
            return false;
    }
}

std::vector<Card> Game::getEliminatedCards() const {
    return m_eliminatedCards;
}

void Game::setEliminatedCards(const std::vector<Card> &m_eliminated_cards) {
    m_eliminatedCards = m_eliminated_cards;
}