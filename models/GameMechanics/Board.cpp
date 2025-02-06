#include "Board.h"
#include "../Actions/Power.h"
#include "../Actions/Wizard.h"

CardPosition Board::findCardIndexes(const Card* _card) const {
    for (short i = 0; i < this->m_board.size(); i++) {
        for (short j = 0; j < this->m_board[i].size(); j++) {
            for (short height = 0; height < this->m_board[i][j].size(); height++) {
                if (&this->m_board[i][j][height] == _card)
                    return { i, j, height };
            }
        }
    }
    return { -1, -1, -1 };
}

Board::Board(size_t _size) {
    this->m_board.resize(_size);
    for (auto& row : this->m_board) {
        row.resize(_size);
        // for (auto& position : row)
            // position.resize(MAX_HEIGHT);
    }
}

Board::Board(nlohmann::json _json) {
    for (const auto& layer1Array : _json["board"]) {
        std::vector<std::deque<Card>> layer1;
        for (const auto& layer2Array : layer1Array) {
            std::deque<Card> layer2;
            for (const auto& card : layer2Array) {
                layer2.emplace_back(card);
            }
            layer1.push_back(layer2);
        }
        m_board.push_back(layer1);
    }
    this->m_firstCardPlayed = _json["started"].get<bool>();
}

size_t Board::getSize() const {
    return this->m_board.size();
}

Matrix<Card>& Board::getBoard() {
    return m_board;
}

bool Board::circularShiftUp(const bool _check) {
    for (std::size_t col = 0; col < this->m_board.size(); ++col) {
        if (!m_board[0][col].empty()) return false;
    }
    if (_check) return true;

    auto temp = std::move(this->m_board[0]);
    for (std::size_t row = 0; row < this->m_board.size() - 1; ++row)
        m_board[row] = std::move(m_board[row + 1]);
    this->m_board[this->m_board.size() - 1] = std::move(temp);

    for (std::size_t col = 0; col < this->m_board.size(); ++col)
        m_board[this->m_board.size() - 1][col].clear();

    return true;
}

bool Board::circularShiftDown(const bool _check) {
    for (std::size_t col = 0; col < this->m_board.size(); ++col) {
        if (!m_board[this->m_board.size() - 1][col].empty()) return false;
    }
    if (_check) return true;

    auto temp = std::move(this->m_board[this->m_board.size() - 1]);
    for (std::size_t row = this->m_board.size() - 1; row > 0; --row)
        m_board[row] = std::move(m_board[row - 1]);
    this->m_board[0] = std::move(temp);

    for (std::size_t col = 0; col < this->m_board.size(); ++col)
        m_board[0][col].clear();

    return true;
}

bool Board::circularShiftLeft(const bool _check) {
    for (std::size_t row = 0; row < this->m_board.size(); ++row) {
        if (!m_board[row][0].empty()) return false;
    }
    if (_check) return true;

    for (std::size_t col = 0; col < this->m_board.size() - 1; ++col) {
        for (std::size_t row = 0; row < this->m_board.size(); ++row)
            m_board[row][col] = std::move(m_board[row][col + 1]);
    }

    for (std::size_t row = 0; row < this->m_board.size(); ++row)
        m_board[row][this->m_board.size() - 1].clear();

    return true;
}

bool Board::circularShiftRight(const bool _check) {
    for (std::size_t row = 0; row < this->m_board.size(); ++row) {
        if (!m_board[row][this->m_board.size() - 1].empty()) return false;
    }
    if (_check) return true;

    for (std::size_t col = this->m_board.size() - 1; col > 0; --col) {
        for (std::size_t row = 0; row < this->m_board.size(); ++row)
            m_board[row][col] = std::move(m_board[row][col - 1]);
    }

    for (std::size_t row = 0; row < this->m_board.size(); ++row)
        m_board[row][0].clear();

    return true;
}

bool Board::checkIfCanShift() {
    return this->circularShiftUp(true) || this->circularShiftDown(true) || this->circularShiftLeft(true) || this->circularShiftRight(true);
}

void Board::printBoard() const {
    for (size_t i = 0; i < this->m_board.size(); ++i) {
        for (size_t j = 0; j < this->m_board.size(); ++j) {
            if (checkHole(i, j))
                std::cout << "HH ";
            else if (!m_board[i][j].empty())
                std::cout << m_board[i][j].back();
            else
                std::cout << "xx ";
        }
        std::cout << std::endl;
    }
}

bool Board::isAPile(const size_t _row, const size_t _col) const {
    return m_board[_row][_col].size() >= 2;
}

bool Board::checkIndexes(const size_t _row, const size_t _col) const {
    return !(_row >= this->m_board.size() || _col >= this->m_board.size());
}

bool Board::checkNeighbours(const size_t _row, const size_t _col) const {
    if (!m_board[_row][_col].empty()) return true;

    const std::array<std::pair<int, int>, 8> directions = { {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}} };
    return std::ranges::any_of(directions, [&](const auto& direction) {
        const int newRow = static_cast<int>(_row) + direction.first;
        const int newCol = static_cast<int>(_col) + direction.second;
        return newRow >= 0 && newRow < this->m_board.size() &&
            newCol >= 0 && newCol < this->m_board.size() &&
            !m_board[newRow][newCol].empty();
        });
}

bool Board::checkValue(const size_t _row, const size_t _col, const Card::Value _value, const bool _illusion) const {
    if (m_board[_row][_col].empty() || m_board[_row][_col].back().getValue() == Card::Value::Border)
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


bool Board::checkHole(size_t _row, size_t _col) const {
    return Explosion::getInstance().getHole() == std::pair{ _row, _col } ||
        Wizard::getInstance().getHole() == std::pair{ _row, _col };
}

bool Board::checkIllusion(const size_t _row, const size_t _col, const Card::Color _color) const {
    if (m_board[_row][_col].empty())
        return false;

    auto& topCard = m_board[_row][_col].back();

    if (topCard.getColor() == Card::Color::Undefined)
        return false;

    return topCard.isIllusion() && topCard.getColor() == _color;
}

bool Board::checkIllusionValue(const size_t _row, const size_t _col, const size_t _value) const {
    if (m_board[_row][_col].empty() || m_board[_row][_col].back().getColor() == Card::Color::Undefined)
        return true;

    return static_cast<size_t>(m_board[_row][_col].back().getValue()) < _value;
}

Card::Color Board::checkWin() const {
    const int n = m_board.size();
    std::vector<int> sums(2 * n + 2, 0);
    size_t chessmanCount = 0;

    for (size_t row = 0; row < n; ++row) {
        for (size_t col = 0; col < n; ++col) {
            if (m_board[row][col].empty())
                continue;

            sums[row] += static_cast<int>(m_board[row][col].back().getColor());
            sums[n + col] += static_cast<int>(m_board[row][col].back().getColor());

            if (row == col)
                sums[2 * n] += static_cast<int>(m_board[row][col].back().getColor());

            if (row == n - 1 - col)
                sums[2 * n + 1] += static_cast<int>(m_board[row][col].back().getColor());

            chessmanCount++;
        }
    }

    for (int sum : sums) {
        if (sum == n) return Card::Color::Red;
        if (sum == -n) return Card::Color::Blue;
    }

    return Card::Color::Undefined;
}

bool Board::checkFullBoard() const {
    for (size_t row = 0; row < this->m_board.size(); ++row)
        for (size_t col = 0; col < this->m_board.size(); ++col)
            if (m_board[row][col].empty())
                return false;

    return true;
}

Card::Color Board::calculateWinner() const {
    std::pair<int, int> winner = { 0, 0 };

    for (std::size_t row = 0; row < this->m_board.size(); ++row) {
        for (std::size_t col = 0; col < this->m_board.size(); ++col) {
            if (!m_board[row][col].empty()) {
                auto& topCard = m_board[row][col].back();

                int cardValue = topCard.isIllusion() ? 1 : static_cast<int>(topCard.getValue()) - 1;
                if (cardValue == static_cast<size_t>(Card::Value::Eter))
                    cardValue = 1;

                if (Power::getInstance().getPlus(*this).x == row && Power::getInstance().getPlus(*this).y == col)
                    cardValue++;

                if (Power::getInstance().getMinus(*this).x == row && Power::getInstance().getMinus(*this).y == col)
                    cardValue--;

                if (topCard.getColor() == Card::Color::Red) {
                    winner.first += cardValue;
                }
                else {
                    winner.second += cardValue;
                }
            }
        }
    }

    if (winner.first > winner.second)
        return Card::Color::Red;

    if (winner.first < winner.second)
        return Card::Color::Blue;

    return Card::Color::Undefined;
}

bool Board::checkTwoRows() const {
    size_t rowCount = 0;
    size_t colCount = 0;

    for (size_t row = 0; row < this->m_board.size(); ++row) {
        bool isRowFull = true;

        for (size_t col = 0; col < this->m_board.size(); ++col)
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

    return rowCount >= 2 || colCount >= 2;
}

void Board::useExplosion(std::vector<Card>& returnedCards, std::vector<Card>& eliminatedCards) {
    bool quit = false;

    do {
        Explosion::getInstance().printExplosion();

        std::cout << "Press 'r' to rotate explosion or 'c' to confirm.\n";
        std::cout << "Press 'x' to to quit using explosion.\n";
    }
    while (!quit && Explosion::getInstance().rotateExplosion(quit));

    if (quit) return;

    auto explosionEffects = Explosion::getInstance().getExplosionEffect();

    std::unordered_map<Explosion::ExplosionEffect, std::function<void(size_t, size_t)>> effectHandlers{
        {Explosion::ExplosionEffect::None, [](size_t,size_t) {} },

        {Explosion::ExplosionEffect::SinkHole, [this, &eliminatedCards](size_t row, size_t col) {
            auto deletedStack = std::move(this->m_board[row][col]);
            this->m_board[row][col].clear();

            if (!this->checkBoardIntegrity())
                this->m_board[row][col] = std::move(deletedStack);
            else {
                Explosion::getInstance().setHole(std::make_pair(row, col));

                for (auto& card : deletedStack) {
                    eliminatedCards.push_back(card);
                }
            }
        }},

        {Explosion::ExplosionEffect::RemoveCard,[this, &eliminatedCards](size_t row, size_t col) {
            Card affectedCard = this->m_board[row][col].back();
            this->m_board[row][col].pop_back();

            if (!this->checkBoardIntegrity())
                this->m_board[row][col].push_back(std::move(affectedCard));
            else
                eliminatedCards.push_back(std::move(affectedCard));
        }},

        {Explosion::ExplosionEffect::ReturnCard,[this, &returnedCards](size_t row, size_t col) {
            Card affectedCard = this->m_board[row][col].back();
            this->m_board[row][col].pop_back();

            if (!this->checkBoardIntegrity())
                this->m_board[row][col].push_back(std::move(affectedCard));
            else
                returnedCards.push_back(std::move(affectedCard));
        }},
    };

    for (size_t row = 0; row < this->getSize(); ++row) {
        for (size_t col = 0; col < this->getSize(); ++col) {
            auto effect = explosionEffects[row][col];

            if (effectHandlers.find(effect) != effectHandlers.end()) {
                if (!this->m_board[row][col].empty() || effect == Explosion::ExplosionEffect::SinkHole)
                    effectHandlers[effect](row, col);
            }
        }
    }
}

bool Board::checkBoardIntegrity() {
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

            for (const auto& [dr, dc] : { std::pair{-1, 0}, {1, 0}, {0, -1}, {0, 1},
                                                                {-1, -1}, {-1, 1}, {1, -1}, {1, 1} }) {
                size_t nr = r + dr, nc = c + dc;
                if (nr < rows && nc < cols && !visited[nr][nc] &&
                    !m_board[nr][nc].empty() &&
                    m_board[nr][nc].back().getColor() != Card::Color::Undefined)
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

    if (!foundFirstSet) m_firstCardPlayed = false;
    return true;
}

bool Board::checkPartial(const size_t _x, const size_t _y, const size_t _int_value) const {
    if (!this->checkIndexes(_x, _y) || this->checkHole(_x, _y))
        return false;

    if (_int_value > static_cast<size_t>(Card::Value::Four))
        return false;

    const auto value = static_cast<Card::Value>(_int_value);

    if (!this->checkValue(_x, _y, value))
        return false;

    if (this->m_firstCardPlayed && !this->checkNeighbours(_x, _y))
        return false;

    const auto& power = Power::getInstance();

    if (_x == power.getRestrictedRow() || _y == power.getRestrictedCol()) {
        return false;
    }

    return true;
}

nlohmann::json Board::toJson() const {
    nlohmann::json json;
    nlohmann::json jsonArray = nlohmann::json::array();

    for (const auto& layer1 : this->m_board) {
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

    json["board"] = jsonArray;
    json["started"] = this->m_firstCardPlayed;

    return json;
}

