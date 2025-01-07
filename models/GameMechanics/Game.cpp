#include "Game.h"

Game::Game(const GameType _gameType, const std::pair<size_t, size_t>& _wizardIndices, const bool _illusions, const bool _explosion, const bool _tournament) :
    m_board{ _gameType == Game::GameType::Training
        ? static_cast<size_t>(GridSize::Three)
        : static_cast<size_t>(GridSize::Four) },

    m_gameType{ _gameType }, m_tournament{ _tournament },
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
     Card::Color::Red,
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
        Card::Color::Blue,
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

Game::Game(GameType _gameType, const nlohmann::json& _json, bool _illusions, bool _explosion, const bool _tournament) :
    m_player1{ _json["player1"] }, m_player2{ _json["player2"] },
    m_gameType{ _gameType }, m_tournament{ _tournament },
    m_board{ _json["board"] } {

    cardPosition playerLastPlacedCard = {
        _json["player1"]["last_placed_card_x"].get<short>(),
        _json["player1"]["last_placed_card_x"].get<short>(),
        _json["player1"]["last_placed_card_x"].get<short>()
    };
    this->m_player1.setLastPlacedCard(
        m_board.getBoard()[playerLastPlacedCard.x][playerLastPlacedCard.y][playerLastPlacedCard.z]);

    playerLastPlacedCard = {
        _json["player2"]["last_placed_card_x"].get<short>(),
        _json["player2"]["last_placed_card_x"].get<short>(),
        _json["player2"]["last_placed_card_x"].get<short>()
    };
    this->m_player2.setLastPlacedCard(
        m_board.getBoard()[playerLastPlacedCard.x][playerLastPlacedCard.y][playerLastPlacedCard.z]);

    this->m_playedExplosion = _json["playedExplosion"].get<bool>();

    this->m_explosionAllowed = _explosion;
    this->m_illusionsAllowed = _illusions;

    for (const auto& card : _json["returnedCards"]) {
        this->m_returnedCards.emplace_back(card);
    }

    for (const auto& card : _json["eliminatedCards"]) {
        this->m_eliminatedCards.emplace_back(card);
    }
}

GameEndInfo Game::run(const bool _player1Turn, bool _timed, int _duration) {
    bool player1Turn = _player1Turn;
    bool endedByCount = false;

    this->m_player1.setTimer(_duration);
    this->m_player2.setTimer(_duration);

    if (this->m_explosionAllowed) {
        Explosion::getInstance().generateExplosion(m_gameType == GameType::Training ? 3 : 4);
        Explosion::getInstance().printExplosion();
    }

    while (running) {
        auto gameEndInfo = checkEndOfGame(!player1Turn ? Card::Color::Red : Card::Color::Blue);
        endedByCount = gameEndInfo.second;

        if (gameEndInfo.first)
            break;

        this->m_board.printBoard();

        auto start_time = std::chrono::high_resolution_clock::now();
        if (auto& player = player1Turn ? m_player1 : m_player2; player.playerTurn(*this)) {
            if (_timed) {
                auto end_time = std::chrono::high_resolution_clock::now();
                auto delta_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

                double delta_seconds = delta_time.count() / 1000.0f;
                delta_seconds = std::round(delta_seconds * 100) / 100;

                if (player.subtractTime(delta_seconds)) {
                    this->m_winner = player.getColor() != Card::Color::Red ? Card::Color::Red : Card::Color::Blue;
                    break;
                }
            }

            player1Turn = !player1Turn;

            runMidRoundLogic();
        }
    }

    if (!running) {
        if (saving) this->saveJson(player1Turn);
        return {m_winner, static_cast<size_t>(-1), static_cast<size_t>(-1)};
    }

    this->m_board.printBoard();

    return this->runEndGameLogic(endedByCount);
}

GameEndInfo Game::run(const nlohmann::json &_json, bool _timed, int _duration) {
    bool player1Turn = _json["playerToPlay"].get<bool>();
    bool endedByCount = false;

    if (this->m_explosionAllowed) {
        Explosion::getInstance().setExplosion(_json["explosion"]);
        Explosion::getInstance().printExplosion();
    }

    while (running) {
        auto gameEndInfo = checkEndOfGame(!player1Turn ? Card::Color::Red : Card::Color::Blue);
        endedByCount = gameEndInfo.second;

        if (gameEndInfo.first)
            break;

        this->m_board.printBoard();

        auto start_time = std::chrono::high_resolution_clock::now();
        if (auto& player = player1Turn ? m_player1 : m_player2; player.playerTurn(*this)) {
            if (_timed) {
                auto end_time = std::chrono::high_resolution_clock::now();
                auto delta_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

                double delta_seconds = delta_time.count() / 1000.0f;
                delta_seconds = std::round(delta_seconds * 100) / 100;

                if (player.subtractTime(delta_seconds)) {
                    this->m_winner = player.getColor() != Card::Color::Red ? Card::Color::Red : Card::Color::Blue;
                    break;
                }
            }

            player1Turn = !player1Turn;

            runMidRoundLogic();
        }
    }

    if (!running) {
        if (saving) this->saveJson(player1Turn);
        return {m_winner, static_cast<size_t>(-1), static_cast<size_t>(-1)};
    }

    this->m_board.printBoard();

    return this->runEndGameLogic(endedByCount);
}

GameEndInfo Game::runEndGameLogic(const bool _endedByCount) {
    if (this->m_board.checkIfCanShift()) {
        char choice;

        while (running) {
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
                    running = false;
                break;
                default:
                    break;
            }

            this->m_board.printBoard();
        }
    }

    size_t x = -1, y = -1;

    if (m_winner == Card::Color::Undefined)
        std::cout << "Draw\n" << std::endl;

    else {
        bool player1Win = this->m_winner == Card::Color::Red;

        std::cout << "Winner: " << (player1Win ? "Red" : "Blue") << " player\n" << std::endl;

        if (!_endedByCount) {
            auto [fst, snd, _] = this->getBoard().findCardIndexes(player1Win ? m_player1.getLastPlacedCard() : m_player2.getLastPlacedCard());

            x = fst;
            y = snd;
        }
    }

    return {m_winner, x, y};
}

void Game::runMidRoundLogic() {
    if (Power::getInstance().getJustBlocked())
        Power::getInstance().setJustBlocked(false);

    else if (Power::getInstance().getRestrictedCol() == -1 || Power::getInstance().getRestrictedRow() == -1) {
        Power::getInstance().setRestrictedCol(-1);
        Power::getInstance().setRestrictedRow(-1);
    }

    if (!m_returnedCards.empty()) {
        for (auto& card : m_returnedCards) {
            card.setJustReturned();

            if (card.getColor() == Card::Color::Red)
                m_player1.returnCard(std::move(card));

            else
                m_player2.returnCard(std::move(card));
        }

        m_returnedCards.clear();
    }
}

bool Game::checkEmptyDeck() const {
    return !m_player1.getCardCount() || !m_player2.getCardCount();
}

std::pair<bool, bool> Game::checkEndOfGame(const Card::Color _color) {
    this->m_winner = this->m_board.checkWin();

    if (this->m_winner != Card::Color::Undefined)
        return {true, false};

    if (checkEmptyDeck() || this->m_board.checkFullBoard()) {
        bool player1Turn = _color == Card::Color::Blue;

        std::cout << (player1Turn ? "Red" : "Blue") << " player's turn!" << std::endl;
        this->m_board.printBoard();

        if (player1Turn) m_player1.playerTurn(*this);
        else m_player2.playerTurn(*this);

        this->m_winner = this->m_board.calculateWinner();

        return {true, true};
    }

    return {false, false};
}

void Game::saveJson(bool _player1Turn) {
    m_json["player1"] = m_player1.toJson(*this);
    m_json["player2"] = m_player2.toJson(*this);

    m_json["playerToPlay"] = _player1Turn;

    nlohmann::json jsonArray = nlohmann::json::array();

    for (const auto& layer1 : this->m_board.m_board) {
        nlohmann::json layer1Array = nlohmann::json::array();
        for (const auto& layer2 : layer1) {
            nlohmann::json layer2Array = nlohmann::json::array();
            for (const auto& card : layer2) {
                layer2Array.push_back(card.toJson());
            }
            layer1Array.push_back(layer2Array);
        }
        jsonArray.push_back(layer1Array);
    }

    m_json["board"] = this->m_board.toJson();

    jsonArray = nlohmann::json::array();
    for (const auto& card : m_returnedCards)
        jsonArray.push_back(card.toJson());

    m_json["returnedCards"] = jsonArray;

    jsonArray = nlohmann::json::array();
    for (const auto& card : m_eliminatedCards)
        jsonArray.push_back(card.toJson());

    m_json["eliminatedCards"] = jsonArray;

    m_json["explosion"] = Explosion::getInstance().serialize();
    m_json["playedExplosion"] = this->m_playedExplosion;

    m_json["wizard"] = Wizard::getInstance().serialize();
    m_json["power"] = Power::getInstance().serialize(*this);
}
