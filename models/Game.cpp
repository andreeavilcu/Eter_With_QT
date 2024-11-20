#include "Game.h"

#include <stack>
#include <bits/ranges_algo.h>

Game::Board::Board(const size_t _size) {
    this->m_board.resize(_size);

    for (auto& row : this->m_board)
        row.resize(_size);
}

size_t Game::Board::getSize() const {
    return this->m_board.size();
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
            if (this->m_hole == std::pair{i, j})
                std::cout << "HH ";

            else if (!m_board[i][j].empty())
                std::cout << m_board[i][j].back();

            else
                std::cout << "xx ";

        std::cout << std::endl;
    }
}

bool Game::Board::isAPile(const size_t _row, const size_t _col) const {
    return m_board[_row][_col].size() >= 2;
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

bool Game::Board::checkValue(const size_t _row, const size_t _col, const Card::Value _value, const bool _illusion) const {
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

bool Game::Board::checkHole(size_t _row, size_t _col) const {
    return this->m_hole == std::pair{ _row, _col };
}

bool Game::Board::checkIllusion(const size_t _row, const size_t _col, const Card::Color _color) const {
    if (m_board[_row][_col].empty())
        return false;

    auto& topCard = m_board[_row][_col].back();

    return topCard.isIllusion() && topCard.getColor() == _color;
}

bool Game::Board::checkIllusionValue(const size_t _row, const size_t _col, const size_t _value) const {
    if (m_board[_row][_col].empty())
        return true;

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

                if (m_plus == std::pair{ row,col })
                    cardValue++;

                if (m_minus == std::pair{ row,col })
                    cardValue--;

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

std::vector<Card> Game::Board::useExplosion(const std::vector<std::vector<ExplosionEffect>> &_matrix) {
    std::vector<Card> returnedCards{};

    for (size_t row = 0; row < this->m_board.size(); ++row) {
        for (size_t col = 0; col < this->m_board.size(); ++col) {
            if (_matrix[row][col] == ExplosionEffect::None)
                continue;

            if (_matrix[row][col] == ExplosionEffect::SinkHole) {
                auto deletedStack = std::move(this->m_board[row][col]);
                this->m_board[row][col].clear();

                if (!this->checkBoardIntegrity())
                    this->m_board[row][col] = std::move(deletedStack);

                else
                    this->m_hole = {row, col};

                continue;
            }

            if (this->m_board[row][col].empty())
                continue;

            Card affectedCard = this->m_board[row][col].back();

            switch (_matrix[row][col]) {
                case ExplosionEffect::RemoveCard:
                    this->m_board[row][col].pop_back();

                    if (!this->checkBoardIntegrity())
                        this->m_board[row][col].push_back(std::move(affectedCard));

                    break;

                case ExplosionEffect::ReturnCard:
                    this->m_board[row][col].pop_back();

                    if (!this->checkBoardIntegrity())
                        this->m_board[row][col].push_back(std::move(affectedCard));

                    else
                        returnedCards.push_back(std::move(affectedCard));

                    break;

                default:
                    break;
            }
        }
    }

    return returnedCards;
}

bool Game::Board::checkBoardIntegrity() const {
    const size_t rows = m_board.size();
    const size_t cols = m_board[0].size();
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));

    bool foundFirstSet = false;

    auto dfs = [&](size_t row, size_t col) {
        std::stack<std::pair<size_t, size_t>> stack;
        stack.emplace(row, col);
        while (!stack.empty()) {
            auto [r, c] = stack.top();
            stack.pop();
            if (visited[r][c]) continue;
            visited[r][c] = true;

            for (const auto& [dr, dc] : {std::pair{-1, 0}, {1, 0}, {0, -1}, {0, 1},
                                                                {-1, -1}, {-1, 1}, {1, -1}, {1, 1}}) {
                size_t nr = r + dr, nc = c + dc;
                if (nr < rows && nc < cols && !visited[nr][nc] && !m_board[nr][nc].empty())
                    stack.emplace(nr, nc);
            }
        }
    };

    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            if (!m_board[row][col].empty() && !visited[row][col]) {
                if (foundFirstSet) return false;
                dfs(row, col);
                foundFirstSet = true;
            }
        }
    }

    return true;
}

void Game::Board::placeCard(const size_t _row, const size_t _col, const Card && _card) {
    m_board[_row][_col].push_back(_card);
}

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

    while (checkEndOfGame(!(iterationIndex % 2) ? Card::Color::Player1 : Card::Color::Player2)) {
        std::cout << "Player " << iterationIndex % 2 + 1 << "'s turn!" << std::endl;

        this->m_board.printBoard();

        if (playerTurn(iterationIndex % 2 ? Card::Color::Player2 : Card::Color::Player1, iterationIndex))
            iterationIndex++;

        if (!this->m_playedExplosion && this->m_board.checkTwoRows())
            this->playExplosion();

        else if (!m_returnedCards.empty()) {
            for (auto& card : m_returnedCards)
                auto _ = std::move(card);

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
    std::vector<std::vector<Game::ExplosionEffect>> explosionEffects = generateExplosion(this->m_board.getSize());

    std::cout << "Player 2's turn\n";
    std::cout << "---------------\n";

    this->m_board.printBoard();

    bool quit = false;

    do {
        printExplosion(explosionEffects);

        std::cout << "Press 'r' to rotate explosion or 'c' to confirm.\n";
        std::cout << "Press 'x' to to quit using explosion.\n";
    }
    while (!quit && rotateExplosion(explosionEffects, quit));

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

std::vector<std::vector<Game::ExplosionEffect>> Game::generateExplosion(const size_t _size) {
    std::random_device rd;
    std::mt19937 gen{ rd() };

    std::uniform_int_distribution<size_t> indexDistribution{ 0, _size - 1};

    std::uniform_int_distribution<size_t> effectDistribution{ 0, 10 };
    std::bernoulli_distribution removeOrReplaceDistribution{ 0.5 };

    std::vector<std::vector<ExplosionEffect>> explosionEffects;

    explosionEffects.resize(_size);
    for (auto& row : explosionEffects)
        row.resize(_size);

    const size_t effectCount = indexDistribution(gen) + (_size - 1);

    for (int _ = 0; _ < effectCount; _++) {
        size_t x, y;

        do {
            x = indexDistribution(gen);
            y = indexDistribution(gen);
        }
        while (explosionEffects[x][y] != ExplosionEffect::None);

        if (const size_t effect = effectDistribution(gen); !effect)
            explosionEffects[x][y] = ExplosionEffect::SinkHole;

        else
            explosionEffects[x][y] = removeOrReplaceDistribution(gen)
                ? ExplosionEffect::ReturnCard
                : ExplosionEffect::RemoveCard;
    }

    return explosionEffects;
}

bool Game::rotateExplosion(std::vector<std::vector<Game::ExplosionEffect>> &_matrix, bool &_quit) {
    char choice;
    std::cin >> choice;

    if (tolower(choice) == 'c')
        return false;

    if (tolower(choice) == 'x') {
        _quit = true;
        return false;
    }

    if (tolower(choice) == 'r')
        rotateMatrixRight(_matrix);

    return true;
}

void Game::rotateMatrixRight(std::vector<std::vector<Game::ExplosionEffect>> &_matrix) {
    const std::vector<std::vector<Game::ExplosionEffect>> temp = _matrix;

    for (int i = 0; i < _matrix.size(); ++i) {
        for (int j = 0; j < _matrix.size(); ++j) {
            _matrix[j][_matrix.size() - 1 - i] = temp[i][j];
        }
    }
}

void Game::printExplosion(const std::vector<std::vector<Game::ExplosionEffect>>& _matrix) const {
    for (size_t i = 0; i < _matrix.size(); ++i) {
        for (size_t j = 0; j < _matrix.size(); ++j) {
            switch (_matrix[i][j]) {
                case ExplosionEffect::None:
                    std::cout << "- ";
                    break;
                case ExplosionEffect::SinkHole:
                    std::cout << "H ";
                    break;
                case ExplosionEffect::RemoveCard:
                    std::cout << "R ";
                    break;
                case ExplosionEffect::ReturnCard:
                    std::cout << "r ";
                    break;
            }
        }

        std::cout << std::endl;
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
