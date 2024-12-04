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
    bool player1Turn = true;

    Power& power = Power::getInstance();

    while (checkEndOfGame(!player1Turn ? Card::Color::Player1 : Card::Color::Player2)) {
        std::cout << "Player " << static_cast<int>(!player1Turn) + 1 << "'s turn!" << std::endl;

        this->m_board.printBoard();

        if (auto& player = player1Turn ? m_player1 : m_player2; player.playerTurn(*this)) {
            player1Turn = !player1Turn;

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

        m_player1.playerTurn(*this);

        this->m_winner = this->m_board.calculateWinner();

        return false;
    }

    return true;
}

bool Game::checkPartial(const size_t _x, const size_t _y, const size_t _int_value) const {
    if (!this->m_board.checkIndexes(_x, _y) || this->m_board.checkHole(_x, _y))
        return false;

    if (_int_value > static_cast<size_t>(Card::Value::Four))
        return false;

    const auto value = static_cast<Card::Value>(_int_value);

    if (!this->m_board.checkValue(_x, _y, value))
        return false;

    if (!this->m_board.checkNeighbours(_x, _y))
        if (!this->m_board.checkBoardIntegrity())
            return false;


    const auto& power = Power::getInstance();

    if (_x == power.getRestrictedRow() || _y == power.getRestrictedCol()) {
        return false;
    }
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

std::vector<Card> Game::getEliminatedCards() const {
    return m_eliminatedCards;
}

void Game::setEliminatedCards(const std::vector<Card> &m_eliminated_cards) {
    m_eliminatedCards = m_eliminated_cards;
}