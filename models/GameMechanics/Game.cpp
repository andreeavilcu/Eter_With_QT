#include "Game.h"

Game::Game(const GameType _gameType, const std::pair<size_t, size_t>& _wizardIndices, const bool _illusions, const bool _explosion) :
    m_board{ _gameType == Game::GameType::Training
        ? static_cast<size_t>(GridSize::Three)
        : static_cast<size_t>(GridSize::Four) },

    m_gameType{ _gameType },
    m_illusionsAllowed{ _illusions },
    m_explosionAllowed{ _explosion },
    m_player1{ Card::Color::Undefined, {}, 0, 0, 0 },
    m_player2{ Card::Color::Undefined, {}, 0, 0, 0 }
{
    std::random_device rd;
    std::mt19937 gen{ rd() };
    std::uniform_int_distribution<size_t> powerDistribution{ 0, Power::power_count - 1 };

    std::pair<size_t, size_t> powerIndices1 = _gameType == GameType::PowerDuel || _gameType == GameType::WizardAndPowerDuel
        ? std::make_pair(powerDistribution(gen), powerDistribution(gen))
        : std::make_pair(static_cast<size_t>(-1), static_cast<size_t>(-1));

    std::pair<size_t, size_t> powerIndices2 = _gameType == GameType::PowerDuel || _gameType == GameType::WizardAndPowerDuel
        ? std::make_pair(powerDistribution(gen), powerDistribution(gen))
        : std::make_pair(static_cast<size_t>(-1), static_cast<size_t>(-1));

   
    m_player1 = Player{
     Card::Color::Player1,
     (_gameType == GameType::Training
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
     _wizardIndices.first,
     powerIndices1.first,
     powerIndices1.second    
    };

    m_player2 = Player{
        Card::Color::Player2,
        (_gameType == GameType::Training
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
        _wizardIndices.second,
        powerIndices2.first,
        powerIndices2.second   
    };
}


GameEndInfo Game::run() {
    bool player1Turn = true;
    bool endedByCount = false;

    if (this->m_explosionAllowed) {
        Explosion::getInstance().generateExplosion(m_gameType == GameType::Training ? 3 : 4);
        Explosion::getInstance().printExplosion();
    }

    while (true) {
        auto gameEndInfo = checkEndOfGame(!player1Turn ? Card::Color::Player1 : Card::Color::Player2);
        endedByCount = gameEndInfo.second;

        if (gameEndInfo.first)
            break;

        std::cout << "Player " << static_cast<int>(!player1Turn) + 1 << "'s turn!" << std::endl;

        this->m_board.printBoard();

        if (auto& player = player1Turn ? m_player1 : m_player2; player.playerTurn(*this)) {
            player1Turn = !player1Turn;

            if (Power::getInstance().getJustBlocked())
                Power::getInstance().setJustBlocked(false);

            else if (Power::getInstance().getRestrictedCol() == -1 || Power::getInstance().getRestrictedRow() == -1) {
                Power::getInstance().setRestrictedCol(-1);
                Power::getInstance().setRestrictedRow(-1);
            }
        }

        if (!m_returnedCards.empty()) {
            for (auto& card : m_returnedCards) {
                card.setJustReturned();

                if (card.getColor() == Card::Color::Player1)
                    m_player1.returnCard(std::move(card));

                else
                    m_player2.returnCard(std::move(card));
            }

            m_returnedCards.clear();
        }
    }

    this->m_board.printBoard();

    if (this->m_board.checkIfCanShift()) {
        char choice;
        bool stop = false;

        while (!stop) {
            std::cout << "Enter direction (wasd) or (x) for exit.\n";
            std::cin >> choice;

            switch (tolower(choice)) {
                case 'w':
                    this->getBoard().circularShiftUp();
                    break;
                case 'a':
                    this->getBoard().circularShiftLeft();
                    break;
                case 's':
                    this->getBoard().circularShiftDown();
                    break;
                case 'd':
                    this->getBoard().circularShiftRight();
                    break;
                case 'x':
                    stop = true;
                default:
                    break;
            }
        }
    }

    size_t x = -1, y = -1;

    if (m_winner == Card::Color::Undefined)
        std::cout << "Draw\n" << std::endl;

    else {
        bool player1Win = this->m_winner == Card::Color::Player1;

        std::cout << "Winner: " << (player1Win ? "Player 1\n" : "Player 2\n") << std::endl;

        if (!endedByCount) {
            auto [fst, snd] = this->getBoard().findCardIndexes(player1Win ? m_player1.getLastPlacedCard() : m_player2.getLastPlacedCard());

            x = fst;
            y = snd;
        }
    }

    return {m_winner, x, y};
}

bool Game::checkEmptyDeck() const {
    return !m_player1.getCardCount() || !m_player2.getCardCount();
}

std::pair<bool, bool> Game::checkEndOfGame(const Card::Color _color) {
    this->m_winner = this->m_board.checkWin();

    if (this->m_winner != Card::Color::Undefined)
        return {true, false};

    if (checkEmptyDeck() || this->m_board.checkFullBoard()) {
        bool player1Turn = _color == Card::Color::Player2;

        std::cout << "Player " << (player1Turn ? 1 : 2) << "'s turn!" << std::endl;
        this->m_board.printBoard();

        if (player1Turn) m_player1.playerTurn(*this);
        else m_player2.playerTurn(*this);

        this->m_winner = this->m_board.calculateWinner();

        return {true, true};
    }

    return {false, false};
}