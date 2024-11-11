#include "Game.h"

Game::Board::Board(const size_t _size) {
    this->m_board.resize(_size);

    for (auto& row : this->m_board)
        row.resize(_size);
}

void Game::Board::circularShiftUp() {
    for (std::size_t col = 0; col < this->m_board.size(); ++col)
        if (!m_board[0][col].empty())
            return;

    for (std::size_t row = 0; row < this->m_board.size() - 1; ++row)
        m_board[row] = std::move(m_board[row + 1]);

    for (std::size_t col = 0; col < this->m_board.size(); ++col)
        m_board[this->m_board.size() - 1][col].clear();
}

void Game::Board::circularShiftDown() {
    for (std::size_t col = 0; col < this->m_board.size(); ++col)
        if (!m_board[this->m_board.size() - 1][col].empty())
            return;

    for (std::size_t row = this->m_board.size() - 1; row > 0; --row)
        m_board[row] = std::move(m_board[row - 1]);

    for (std::size_t col = 0; col < this->m_board.size(); ++col)
        m_board[0][col].clear();
}

void Game::Board::circularShiftLeft() {
    for (std::size_t row = 0; row < this->m_board.size(); ++row)
        if (!m_board[row][0].empty())
            return;

    for (std::size_t col = 0; col < this->m_board.size() - 1; ++col)
        for (std::size_t row = 0; row < this->m_board.size(); ++row)
            m_board[row][col] = std::move(m_board[row][col + 1]);

    for (std::size_t row = 0; row < this->m_board.size(); ++row)
        m_board[row][this->m_board.size() - 1].clear();
}

void Game::Board::circularShiftRight() {
    for (std::size_t row = 0; row < this->m_board.size(); ++row)
        if (!m_board[row][this->m_board.size() - 1].empty())
            return;

    for (std::size_t col = this->m_board.size() - 1; col > 0; --col)
        for (std::size_t row = 0; row < this->m_board.size(); ++row)
            m_board[row][col] = std::move(m_board[row][col - 1]);

    for (std::size_t row = 0; row < this->m_board.size(); ++row)
        m_board[row][0].clear();
}

void Game::Board::printBoard() const {
    for (size_t i = 0; i < this->m_board.size(); ++i) {
        for (size_t j = 0; j < this->m_board.size(); ++j)
            if (!m_board[i][j].empty())
                std::cout << m_board[i][j].back() << " ";

            else
                std::cout << "xx" << " ";

        std::cout << std::endl;
    }
}

bool Game::Board::checkIndexes(const size_t _row, const size_t _col) const {
    return !(_row >= this->m_board.size() || _col >= this->m_board.size());
}

bool Game::Board::checkNeighbours(const size_t _row, const size_t _col) const {
    if (!m_board[_row][_col].empty())
        return true;

    const std::array<std::pair<int, int>, 8> directions = {{
        {-1, 0}, {1, 0}, {0, -1}, {0, 1},
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
    }};

    return std::ranges::any_of(directions, [&](const auto& direction) {
        const int newRow = static_cast<int>(_row) + direction.first;
        const int newCol = static_cast<int>(_col) + direction.second;

        return newRow >= 0 && newRow < this->m_board.size() &&
               newCol >= 0 && newCol < this->m_board.size() &&
                   !m_board[newRow][newCol].empty();
    });
}

bool Game::Board::checkValue(const size_t _row, const size_t _col, const Card::Value &_value, bool _illusion) const {
    if (m_board[_row][_col].empty())
        return true;

    if (_illusion)
        return false;

    if (m_board[_row][_col].back().getValue() == Card::Value::Eter || _value == Card::Value::Eter)
        return false;

    if (m_board[_row][_col].back().isIllusion())
        return true;

    if (m_board[_row][_col].back().getValue() < _value)
        return true;

    return false;
}

bool Game::Board::checkIllusion(size_t _row, size_t _col, Card::Color _color) const {
    if (m_board[_row][_col].empty())
        return false;

    auto& topCard = m_board[_row][_col].back();

    return topCard.isIllusion() && topCard.getColor() == _color;
}

bool Game::Board::checkIllusionValue(const size_t _row, const size_t _col, const size_t _value) const {
    return static_cast<size_t>(m_board[_row][_col].back().getValue()) < _value;
}

void Game::Board::resetIllusion(const size_t _row, const size_t _col) {
    this->m_board[_row][_col].back().resetIllusion();
}

Card::Color Game::Board::checkRows() const {
    for (std::size_t row = 0; row < this->m_board.size(); ++row) {
        if (m_board[row][0].empty())
            continue;

        Card::Color rowColor = m_board[row][0].back().getColor();

        for (std::size_t col = 1; col < this->m_board.size(); ++col)
            if (m_board[row][col].empty() || m_board[row][col].back().getColor() != rowColor) {
                rowColor = Card::Color::Undefined;
                break;
            }

        if (rowColor != Card::Color::Undefined)
            return rowColor;
    }

    return Card::Color::Undefined;
}

Card::Color Game::Board::checkCols() const {
    for (std::size_t col = 0; col< this->m_board.size(); ++col) {
        if (m_board[0][col].empty())
            continue;

        Card::Color colColor = m_board[0][col].back().getColor();

        for (std::size_t row = 1; row < this->m_board.size(); ++row)
            if (m_board[row][col].empty() || m_board[row][col].back().getColor() != colColor) {
                colColor = Card::Color::Undefined;
                break;
            }

        if (colColor != Card::Color::Undefined)
            return colColor;
    }

    return Card::Color::Undefined;
}

Card::Color Game::Board::checkDiagonals() const {
    if (m_board[0][0].empty())
        return Card::Color::Undefined;

    Card::Color diagColor = m_board[0][0].back().getColor();

    for (size_t index = 1; index < this->m_board.size(); ++index) {
        if (m_board[index][index].empty() || m_board[index][index].back().getColor() != diagColor)
            return Card::Color::Undefined;
    }

    if (diagColor != Card::Color::Undefined)
        return diagColor;

    if (m_board[0][this->m_board.size() - 1].empty())
        return Card::Color::Undefined;

    diagColor = m_board[0][this->m_board.size() - 1].back().getColor();

    for (size_t index = 1; index < this->m_board.size(); ++index) {
        if (m_board[index][index].empty() || m_board[index][this->m_board.size() - index - 1].back().getColor() != diagColor)
            return Card::Color::Undefined;
    }

    return diagColor;
}

bool Game::Board::checkFullBoard() const {
    for (size_t row = 0; row < this->m_board.size(); ++row)
        for (size_t col = 0; col < this->m_board.size(); ++col)
            if (m_board[row][col].empty())
                return false;

    return true;
}

Card::Color Game::Board::calculateWinner() const {
    std::pair<int, int> winner = {0, 0};

    for (std::size_t row = 0; row < this->m_board.size(); ++row) {
        for (std::size_t col = 0; col < this->m_board.size(); ++col) {
            if (!m_board[row][col].empty()) {
                auto& topCard = m_board[row][col].back();

                int cardValue =  topCard.isIllusion() ? 1 : static_cast<int>(topCard.getValue());
                if (cardValue == static_cast<size_t>(Card::Value::Eter))
                    cardValue = 1;

                if (topCard.getColor() == Card::Color::Player1) {
                    winner.first += cardValue;
                } else {
                    winner.second += cardValue;
                }
            }
        }
    }

    if (winner.first > winner.second)
        return Card::Color::Player1;

    if (winner.first < winner.second)
        return Card::Color::Player2;

    return Card::Color::Undefined;
}

bool Game::Board::checkTwoRows() const {
    size_t rowCount = 0;
    size_t colCount = 0;

    for (size_t row = 0; row < this->m_board.size(); ++row) {
        bool isRowFull = true;

        for (size_t col = 0; !this->m_board.empty(); ++col)
            if (m_board[row][col].empty()) {
                isRowFull = false;
                break;
        }

        if (isRowFull)
            ++rowCount;
    }

    for (size_t col = 0; col < this->m_board.size(); ++col) {
        bool isColFull = true;

        for (size_t row = 0; row < this->m_board.size(); ++row) {
            if (m_board[row][col].empty()) {
                isColFull = false;
                break;
            }
        }

        if (isColFull)
            ++colCount;
    }

    return (rowCount >= 2 || colCount >= 2);
}

void Game::Board::placeCard(const size_t _row, const size_t _col, const Card && _card) {
    m_board[_row][_col].push_back(_card);
}

Game::Game(GameType _gameType) :
    m_board(_gameType == Game::GameType::Training
       ? static_cast<size_t>(GridSize::Three)
       : static_cast<size_t>(GridSize::Four)),

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

    while (checkEndOfGame(!(iterationIndex % 2) ? Card::Color::Player1 : Card::Color::Player2)) {
        std::cout << "Player " << iterationIndex % 2 + 1 << "'s turn!" << std::endl;

        this->m_board.printBoard();

        if (playerTurn(iterationIndex % 2 ? Card::Color::Player2 : Card::Color::Player1, iterationIndex))
            iterationIndex++;

        if (!this->m_playedExplosion && this->m_board.checkTwoRows()) {
            //if functie verificare play explosion (in
            //functie playexplsion - afiseaza pe ecran daca vreau sa joc explozia - alta functie care genereaza explozia
            m_playedExplosion = true;
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

bool Game::checkEndOfGame(const Card::Color _color) {
    this->m_winner = this->m_board.checkRows();

    if (this->m_winner == Card::Color::Undefined)
        this->m_winner = this->m_board.checkCols();

    if (this->m_winner == Card::Color::Undefined)
        this->m_winner = this->m_board.checkDiagonals();

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

bool Game::playCard(const Card::Color _color, const size_t _iterationIndex) {
    size_t x, y, int_value;

    std::cin >> x;
    std::cin >> y;
    std::cin >> int_value;

    if (!this->m_board.checkIndexes(x, y))
        return false;

    if (int_value > static_cast<size_t>(Card::Value::Four))
        return false;

    const auto value = static_cast<Card::Value>(int_value);

    if (!this->m_board.checkValue(x, y, value))
        return false;

    if (_iterationIndex && !this->m_board.checkNeighbours(x, y))
        return false;

    if (this->m_board.checkIllusion(x, y, _color))
        return false;

    auto playedCard =
        m_player1.getColor() == _color ?
            m_player1.useCard(value) :
            m_player2.useCard(value);

    if (!playedCard)
        return false;

    if (!this->m_board.checkIllusion(x, y, Card::Color::Undefined) && this->m_board.checkIllusionValue(x, y, int_value))
        this->m_board.placeCard(x, y, std::move(*playedCard));

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

    if (!this->m_board.checkIndexes(x, y))
        return false;

    if (int_value > static_cast<size_t>(Card::Value::Four))
        return false;

    const auto value = static_cast<Card::Value>(int_value);

    if (!this->m_board.checkValue(x, y, value))
        return false;

    if (_iterationIndex && !this->m_board.checkNeighbours(x, y))
        return false;

    if (this->m_board.checkIllusion(x, y, Card::Color::Player1) || this->m_board.checkIllusion(x, y, Card::Color::Player2))
        return false;

    auto playedCard =
        _color == Card::Color::Player1 ?
            m_player1.useIllusion(value) :
            m_player2.useIllusion(value);

    if (!playedCard)
        return false;

    this->m_board.placeCard(x, y, std::move(*playedCard));

    return true;
}

void Game::playExplosion() {
}

bool Game::playerTurn(const Card::Color _color, const size_t _iterationIndex) {
    char choice;

    std::cout << "Play a card (c)\n";
    std::cout << "Shift board (s)\n";
    std::cout << "Play wizard (w)\n";
    std::cout << "Play power  (p)\n";
    std::cout << "Play illusion (i)\n";
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
            // TODO add checks for empty table, possibility to play wizard, etc.
        case 'p': // TODO add checks for empty table, possibility to play power, etc. and query for which power to play
        default:
            return false;
    }
}
