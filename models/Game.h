#pragma once

#include <iostream>
#include <utility>
#include <vector>
#include <memory>
#include <ranges>
#include <array>

#include "Player.h"

enum class GameType : size_t {
    Training = 0,
    WizardDuel,
    PowerDuel,
    WizardAndPowerDuel,
};

enum class GridSize : size_t {
    Three = 3,
    Four = 4,
};

template<GridSize gridSize>
class Game {
private:
    Game() = default;
    ~Game() = default;

public:
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    static Game& getInstance() {
        static Game<gridSize> instance;
        return instance;
    }

private:
    std::array<std::array<std::vector<Card>, static_cast<size_t>(gridSize)>, static_cast<size_t>(gridSize)> m_board{};

public:
    void swapRow(size_t _first, size_t _second);
    void swapCol(size_t _first, size_t _second);

    void circularShiftUp();
    void circularShiftDown();
    void circularShiftLeft();
    void circularShiftRight();

    void printBoard() const;

    [[nodiscard]] bool checkIndexes(size_t _row, size_t _col) const;
    [[nodiscard]] bool checkNeighbours(size_t _row, size_t _col) const;
    [[nodiscard]] bool checkValue(size_t _row, size_t _col, const Card::Value& _value) const;

    void shiftBoard();

    bool placeCard(Player& _player, size_t _iterationIndex);
    bool playerTurn(Player& _player, size_t _iterationIndex);

    void run(GameType _mode);

};

template<GridSize gridSize>
void Game<gridSize>::swapRow(const size_t _first, const size_t _second) { // TODO test
    if (_first < static_cast<size_t>(gridSize) && _second < static_cast<size_t>(gridSize)) {
        std::ranges::swap(m_board[_first], m_board[_second]);
    }
}

template<GridSize gridSize>
void Game<gridSize>::swapCol(const size_t _first, const size_t _second) { // TODO test
    if (_first < static_cast<size_t>(gridSize) && _second < static_cast<size_t>(gridSize)) {
        for (size_t i = 0; i < static_cast<size_t>(gridSize); ++i) {
            std::ranges::swap(m_board[i][_first], m_board[i][_second]);
        }
    }
}

template<GridSize gridSize>
void Game<gridSize>::circularShiftUp() {
    for (std::size_t col = 0; col < static_cast<std::size_t>(gridSize); ++col)
        if (!m_board[0][col].empty())
            return;

    for (std::size_t row = 0; row < static_cast<std::size_t>(gridSize) - 1; ++row)
        m_board[row] = std::move(m_board[row + 1]);

    for (std::size_t col = 0; col < static_cast<std::size_t>(gridSize); ++col)
        m_board[static_cast<std::size_t>(gridSize) - 1][col].clear();
}

template<GridSize gridSize>
void Game<gridSize>::circularShiftDown() {
    for (std::size_t col = 0; col < static_cast<std::size_t>(gridSize); ++col)
        if (!m_board[static_cast<std::size_t>(gridSize) - 1][col].empty())
            return;

    for (std::size_t row = static_cast<std::size_t>(gridSize) - 1; row > 0; --row)
        m_board[row] = std::move(m_board[row - 1]);

    for (std::size_t col = 0; col < static_cast<std::size_t>(gridSize); ++col)
        m_board[0][col].clear();
}

template<GridSize gridSize>
void Game<gridSize>::circularShiftLeft() {
    for (std::size_t row = 0; row < static_cast<std::size_t>(gridSize); ++row)
        if (!m_board[row][0].empty())
            return;

    for (std::size_t col = 0; col < static_cast<std::size_t>(gridSize) - 1; ++col)
        for (std::size_t row = 0; row < static_cast<std::size_t>(gridSize); ++row)
            m_board[row][col] = std::move(m_board[row][col + 1]);

    for (std::size_t row = 0; row < static_cast<std::size_t>(gridSize); ++row)
        m_board[row][static_cast<std::size_t>(gridSize) - 1].clear();
}

template<GridSize gridSize>
void Game<gridSize>::circularShiftRight() {
    for (std::size_t row = 0; row < static_cast<std::size_t>(gridSize); ++row)
        if (!m_board[row][static_cast<std::size_t>(gridSize) - 1].empty())
            return;

    for (std::size_t col = static_cast<std::size_t>(gridSize) - 1; col > 0; --col)
        for (std::size_t row = 0; row < static_cast<std::size_t>(gridSize); ++row)
            m_board[row][col] = std::move(m_board[row][col - 1]);

    for (std::size_t row = 0; row < static_cast<std::size_t>(gridSize); ++row)
        m_board[row][0].clear();
}

template<GridSize gridSize>
void Game<gridSize>::printBoard() const {
    for (size_t i = 0; i < static_cast<size_t>(gridSize); ++i) {
        for (size_t j = 0; j < static_cast<size_t>(gridSize); ++j) {
            if (!m_board[i][j].empty()) {
                std::cout << m_board[i][j].back() << " ";
            } else {
                std::cout << 'x' << " ";
            }
        }
        std::cout << std::endl;
    }
}

template<GridSize gridSize>
bool Game<gridSize>::checkIndexes(const size_t _row, const size_t _col) const {
    if (_row >= static_cast<size_t>(gridSize) || _col >= static_cast<size_t>(gridSize))
        return false;

    return true;
}

template<GridSize gridSize>
bool Game<gridSize>::checkNeighbours(size_t _row, size_t _col) const {
    if (!m_board[_row][_col].empty())
        return true;

    const std::array<std::pair<int, int>, 8> directions = {{
        {-1, 0}, {1, 0}, {0, -1}, {0, 1},
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
    }};

    return std::ranges::any_of(directions, [&](const auto& direction) {
        int newRow = static_cast<int>(_row) + direction.first;
        int newCol = static_cast<int>(_col) + direction.second;

        return newRow >= 0 && newRow < static_cast<int>(gridSize) &&
               newCol >= 0 && newCol < static_cast<int>(gridSize) &&
               !m_board[newRow][newCol].empty();
    });
}

template<GridSize gridSize>
bool Game<gridSize>::checkValue(size_t _row, size_t _col, const Card::Value& _value) const {
    if (m_board[_row][_col].empty())
        return true;

    if (m_board[_row][_col].back().getValue() == Card::Value::Eter || _value == Card::Value::Eter)
        return false;

    if (m_board[_row][_col].back().getValue() < _value)
        return true;

    return false;
}

template<GridSize gridSize>
void Game<gridSize>::shiftBoard() {
    char choice;

    std::cin >> choice;

    switch (choice) {
        case 'w':
            circularShiftUp();
            break;
        case 'a':
            circularShiftLeft();
            break;
        case 's':
            circularShiftDown();
            break;
        case 'd':
            circularShiftRight();
            break;
        default:
            break;
    }
}

template<GridSize gridSize>
bool Game<gridSize>::placeCard(Player& _player, const size_t _iterationIndex) {
    size_t x, y, int_value;

    std::cin >> x;
    std::cin >> y;
    std::cin >> int_value;

    if (!checkIndexes(x, y))
        return false;

    if (int_value > static_cast<size_t>(Card::Value::Four))
        return false;

    const auto value = static_cast<Card::Value>(int_value);

    if (!checkValue(x, y, value))
        return false;

    if (_iterationIndex && !checkNeighbours(x, y))
        return false;

    auto playedCard = _player.useCard(value);

    if (!playedCard)
        return false;

    m_board[x][y].push_back(std::move(*playedCard));

    return true;
}

template<GridSize gridSize>
bool Game<gridSize>::playerTurn(Player& _player, const size_t _iterationIndex) {
    char choice;

    std::cout << "Play a card (c)\n";
    std::cout << "Shift board (s)\n";
    std::cout << "Play wizard (w)\n";
    std::cout << "Play power  (p)\n";
    std::cin >> choice;

    switch (choice) {
        case 'c':
            return placeCard(_player, _iterationIndex);
        case 's':
            shiftBoard();
            return false;
        case 'w': // TODO add checks for empty table, possibility to play wizard, etc.
            return _player.useWizard();
        case 'p': // TODO add checks for empty table, possibility to play power, etc. and query for which power to play
            _player.usePower(true);
            return _player.usePower(false);
        default:
            return false;
    }
}

template<GridSize gridSize>
void Game<gridSize>::run(const GameType _mode) {
    std::vector<Card> cards = {
        Card{ Card::Value::One }, Card{ Card::Value::One },
        Card{ Card::Value::Two }, Card{ Card::Value::Two },
        Card{ Card::Value::Three }, Card{ Card::Value::Three },
        Card{ Card::Value::Four }
    };

    if (_mode != GameType::Training) {
        cards.emplace_back( Card::Value::Eter );
        cards.emplace_back( Card::Value::Two );
        cards.emplace_back( Card::Value::Three );
    }

    const std::vector<Card> cards_bc = {
        Card{ Card::Value::Eter }, Card{ Card::Value::One },
        Card{ Card::Value::One }, Card{ Card::Value::Two },
        Card{ Card::Value::Two }, Card{ Card::Value::Two },
        Card{ Card::Value::Three }, Card{ Card::Value::Three },
        Card{ Card::Value::Three }, Card{ Card::Value::Four }
    };

    Player player1{ Player::Color::Red, cards,
        _mode == GameType::WizardDuel || _mode == GameType::WizardAndPowerDuel,
        _mode == GameType::PowerDuel || _mode == GameType::WizardAndPowerDuel };
    Player player2{ Player::Color::Blue, cards,

        _mode == GameType::WizardDuel || _mode == GameType::WizardAndPowerDuel,
        _mode == GameType::PowerDuel || _mode == GameType::WizardAndPowerDuel };

    size_t iterationIndex = 0;

    while (iterationIndex < 10) { // TODO remove constant 10 and add some checks like full table or empty decks for both players or game end
        std::cout << "Player " << iterationIndex % 2 + 1 << "'s turn!" << std::endl;

        printBoard();

        if (playerTurn(iterationIndex % 2 ? player1 : player2, iterationIndex))
            iterationIndex++;
    }
}
