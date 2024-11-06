#pragma once

#include <iostream>
#include <cstdlib>
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

template<GridSize gridSize, GameType gameType>
class Game {
private:
    Game();
    ~Game() = default;

public:
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    static Game& getInstance() {
        static Game<gridSize, gameType> instance;
        return instance;
    }

private:
    std::array<std::array<std::vector<Card>, static_cast<size_t>(gridSize)>, static_cast<size_t>(gridSize)> m_board{};

    Player m_player1, m_player2;
    Card::Color m_winner{ Card::Color::Undefined };

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
    
    [[nodiscard]] Card::Color checkRows() const;
    [[nodiscard]] Card::Color checkCols() const;
    [[nodiscard]] Card::Color checkDiagonals() const;

    [[nodiscard]] bool checkFullBoard() const;
    [[nodiscard]] bool checkEmptyDeck() const;

    [[nodiscard]] Card::Color calculateWinner() const;

    [[nodiscard]] bool checkEndOfGame(Card::Color _color);
   
    void shiftBoard();

    bool placeCard(Card::Color _color, size_t _iterationIndex);
    bool playerTurn(Card::Color _color, size_t _iterationIndex);

    void run();

};

template<GridSize gridSize, GameType gameType>
Game<gridSize, gameType>::Game() :
m_player1{ Card::Color::Blue,  gameType == GameType::Training ?
    std::vector<Card>{
        Card{ Card::Value::One }, Card{ Card::Value::One },
        Card{ Card::Value::Two }, Card{ Card::Value::Two },
        Card{ Card::Value::Three }, Card{ Card::Value::Three },
        Card{ Card::Value::Four }
    } :
    std::vector<Card>{
        Card{ Card::Value::Eter }, Card{ Card::Value::One },
        Card{ Card::Value::Two }, Card{ Card::Value::Two },
        Card{ Card::Value::Two }, Card{ Card::Value::Three },
        Card{ Card::Value::Three }, Card{ Card::Value::Three },
        Card{ Card::Value::Three}, Card{ Card::Value::Four }
    }, gameType == GameType::WizardDuel || gameType == GameType::WizardAndPowerDuel,
        gameType == GameType::PowerDuel || gameType == GameType::WizardAndPowerDuel},
    m_player2{ Card::Color::Red,  gameType == GameType::Training ?
        std::vector<Card>{
            Card{ Card::Value::One }, Card{ Card::Value::One },
            Card{ Card::Value::Two }, Card{ Card::Value::Two },
            Card{ Card::Value::Three }, Card{ Card::Value::Three },
            Card{ Card::Value::Four }
        } :
        std::vector<Card>{
            Card{ Card::Value::Eter }, Card{ Card::Value::One },
            Card{ Card::Value::Two }, Card{ Card::Value::Two },
            Card{ Card::Value::Two }, Card{ Card::Value::Three },
            Card{ Card::Value::Three }, Card{ Card::Value::Three },
            Card{ Card::Value::Three}, Card{ Card::Value::Four }
        }, gameType == GameType::WizardDuel || gameType == GameType::WizardAndPowerDuel,
            gameType == GameType::PowerDuel || gameType == GameType::WizardAndPowerDuel} {
}


template<GridSize gridSize, GameType gameType>
void Game<gridSize, gameType>::swapRow(const size_t _first, const size_t _second) { // TODO test
    if (_first < static_cast<size_t>(gridSize) && _second < static_cast<size_t>(gridSize)) {
        std::ranges::swap(m_board[_first], m_board[_second]);
    }
}

template<GridSize gridSize, GameType gameType>
void Game<gridSize, gameType>::swapCol(const size_t _first, const size_t _second) { // TODO test
    if (_first < static_cast<size_t>(gridSize) && _second < static_cast<size_t>(gridSize)) {
        for (size_t i = 0; i < static_cast<size_t>(gridSize); ++i) {
            std::ranges::swap(m_board[i][_first], m_board[i][_second]);
        }
    }
}

template<GridSize gridSize, GameType gameType>
void Game<gridSize, gameType>::circularShiftUp() {
    for (std::size_t col = 0; col < static_cast<std::size_t>(gridSize); ++col)
        if (!m_board[0][col].empty())
            return;

    for (std::size_t row = 0; row < static_cast<std::size_t>(gridSize) - 1; ++row)
        m_board[row] = std::move(m_board[row + 1]);

    for (std::size_t col = 0; col < static_cast<std::size_t>(gridSize); ++col)
        m_board[static_cast<std::size_t>(gridSize) - 1][col].clear();
}

template<GridSize gridSize, GameType gameType>
void Game<gridSize, gameType>::circularShiftDown() {
    for (std::size_t col = 0; col < static_cast<std::size_t>(gridSize); ++col)
        if (!m_board[static_cast<std::size_t>(gridSize) - 1][col].empty())
            return;

    for (std::size_t row = static_cast<std::size_t>(gridSize) - 1; row > 0; --row)
        m_board[row] = std::move(m_board[row - 1]);

    for (std::size_t col = 0; col < static_cast<std::size_t>(gridSize); ++col)
        m_board[0][col].clear();
}

template<GridSize gridSize, GameType gameType>
void Game<gridSize, gameType>::circularShiftLeft() {
    for (std::size_t row = 0; row < static_cast<std::size_t>(gridSize); ++row)
        if (!m_board[row][0].empty())
            return;

    for (std::size_t col = 0; col < static_cast<std::size_t>(gridSize) - 1; ++col)
        for (std::size_t row = 0; row < static_cast<std::size_t>(gridSize); ++row)
            m_board[row][col] = std::move(m_board[row][col + 1]);

    for (std::size_t row = 0; row < static_cast<std::size_t>(gridSize); ++row)
        m_board[row][static_cast<std::size_t>(gridSize) - 1].clear();
}

template<GridSize gridSize, GameType gameType>
void Game<gridSize, gameType>::circularShiftRight() {
    for (std::size_t row = 0; row < static_cast<std::size_t>(gridSize); ++row)
        if (!m_board[row][static_cast<std::size_t>(gridSize) - 1].empty())
            return;

    for (std::size_t col = static_cast<std::size_t>(gridSize) - 1; col > 0; --col)
        for (std::size_t row = 0; row < static_cast<std::size_t>(gridSize); ++row)
            m_board[row][col] = std::move(m_board[row][col - 1]);

    for (std::size_t row = 0; row < static_cast<std::size_t>(gridSize); ++row)
        m_board[row][0].clear();
}

template<GridSize gridSize, GameType gameType>
void Game<gridSize, gameType>::printBoard() const {
    for (size_t i = 0; i < static_cast<size_t>(gridSize); ++i) {
        for (size_t j = 0; j < static_cast<size_t>(gridSize); ++j) {
            if (!m_board[i][j].empty())
                std::cout << m_board[i][j].back() << " ";

            else
                std::cout << 'x' << " ";
        }
        std::cout << std::endl;
    }
}

template<GridSize gridSize, GameType gameType>
bool Game<gridSize, gameType>::checkIndexes(const size_t _row, const size_t _col) const {
    return !(_row >= static_cast<size_t>(gridSize) || _col >= static_cast<size_t>(gridSize));
}

template<GridSize gridSize, GameType gameType>
bool Game<gridSize, gameType>::checkNeighbours(size_t _row, size_t _col) const {
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

template<GridSize gridSize, GameType gameType>
bool Game<gridSize, gameType>::checkValue(size_t _row, size_t _col, const Card::Value& _value) const {
    if (m_board[_row][_col].empty())
        return true;

    if (m_board[_row][_col].back().getValue() == Card::Value::Eter || _value == Card::Value::Eter)
        return false;

    if (m_board[_row][_col].back().getValue() < _value)
        return true;

    return false;
}

template<GridSize gridSize, GameType gameType>
Card::Color Game<gridSize, gameType>::checkRows() const
{
    for (std::size_t row = 0; row < static_cast<std::size_t>(gridSize); ++row) {
        if (m_board[row][0].empty())
            continue;

        Card::Color rowColor = m_board[row][0].back().getColor();

        for (std::size_t col = 1; col < static_cast<std::size_t>(gridSize); ++col)
            if (m_board[row][col].empty() || m_board[row][col].back().getColor() != rowColor) {
                rowColor = Card::Color::Undefined;
                break;
            }

        if (rowColor != Card::Color::Undefined)
            return rowColor;
    }

    return Card::Color::Undefined;
}

template<GridSize gridSize, GameType gameType>
Card::Color Game<gridSize, gameType>::checkCols() const {
    for (std::size_t col = 0; col< static_cast<std::size_t>(gridSize); ++col) {
        if (m_board[0][col].empty())
            continue;

        Card::Color colColor = m_board[0][col].back().getColor();

        for (std::size_t row = 1; row < static_cast<std::size_t>(gridSize); ++row)
            if (m_board[row][col].empty() || m_board[row][col].back().getColor() != colColor) {
                colColor = Card::Color::Undefined;
                break;
            }

        if (colColor != Card::Color::Undefined)
            return colColor;
    }

    return Card::Color::Undefined;
}

template<GridSize gridSize, GameType gameType>
Card::Color Game<gridSize, gameType>::checkDiagonals() const {
    if (m_board[0][0].empty())
        return Card::Color::Undefined;

    Card::Color diagColor = m_board[0][0].back().getColor();

    for (size_t index = 1; index < static_cast<size_t>(gridSize); ++index) {
        if (m_board[index][index].empty() || m_board[index][index].back().getColor() != diagColor)
            return Card::Color::Undefined;
    }

    if (diagColor != Card::Color::Undefined)
        return diagColor;

    if (m_board[0][static_cast<size_t>(gridSize) - 1].empty())
        return Card::Color::Undefined;

    diagColor = m_board[0][static_cast<size_t>(gridSize) - 1].back().getColor();

    for (size_t index = 1; index < static_cast<size_t>(gridSize); ++index) {
        if (m_board[index][index].empty() || m_board[index][static_cast<std::size_t>(gridSize) - index - 1].back().getColor() != diagColor)
            return Card::Color::Undefined;
    }

    return diagColor;
}

template<GridSize gridSize, GameType gameType>
bool Game<gridSize, gameType>::checkFullBoard() const {
    for (size_t row = 0; row < static_cast<size_t>(gridSize); ++row)
        for (size_t col = 0; col < static_cast<size_t>(gridSize); ++col)
            if (m_board[row][col].empty())
                return false;

    return true;
}

template<GridSize gridSize, GameType gameType>
bool Game<gridSize, gameType>::checkEmptyDeck() const {
    return !m_player1.getCardCount() || !m_player2.getCardCount();
}

template<GridSize gridSize, GameType gameType>
Card::Color Game<gridSize, gameType>::calculateWinner() const {
    std::pair<int, int> winner = {0, 0};

    for (std::size_t row = 0; row < static_cast<std::size_t>(gridSize); ++row)
        for (std::size_t col = 0; col < static_cast<std::size_t>(gridSize); ++col)
            if (!m_board[row][col].empty()) {
                if (m_board[row][col].back().getColor() == m_player1.getColor())
                    winner.first += static_cast<int>(m_board[row][col].back().getValue());
                else
                    winner.second += static_cast<int>(m_board[row][col].back().getValue());
            }

    if (winner.first > winner.second)
        return m_player1.getColor();

    if (winner.first < winner.second)
        return m_player2.getColor();

    return Card::Color::Undefined;
}

template<GridSize gridSize, GameType gameType>
bool Game<gridSize, gameType>::checkEndOfGame(const Card::Color _color) {
    this->m_winner = checkRows();
    this->m_winner = checkCols();
    this->m_winner = checkDiagonals();

    if (this->m_winner != Card::Color::Undefined)
        return false;

    if (checkEmptyDeck() || checkFullBoard()) {
        playerTurn(this->m_player1.getColor() == _color ? this->m_player1.getColor() : this->m_player2.getColor(), -1);

        this->m_winner = calculateWinner();

        return false;
    }

    return true;
}

template<GridSize gridSize, GameType gameType>
void Game<gridSize, gameType>::shiftBoard() {
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

template<GridSize gridSize, GameType gameType>
bool Game<gridSize, gameType>::placeCard(const Card::Color _color, const size_t _iterationIndex) {
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

    auto playedCard =
        m_player1.getColor() == _color ?
            m_player1.useCard(value) :
            m_player2.useCard(value);

    if (!playedCard)
        return false;

    m_board[x][y].push_back(std::move(*playedCard));

    return true;
}

template<GridSize gridSize, GameType gameType>
bool Game<gridSize, gameType>::playerTurn(const Card::Color _color, const size_t _iterationIndex) {
    char choice;

    std::cout << "Play a card (c)\n";
    std::cout << "Shift board (s)\n";
    std::cout << "Play wizard (w)\n";
    std::cout << "Play power  (p)\n";
    std::cin >> choice;

    switch (choice) {
        case 'c':
            return placeCard(_color, _iterationIndex);
        case 's':
            shiftBoard();
            return false;
        case 'w': // TODO add checks for empty table, possibility to play wizard, etc.
        case 'p': // TODO add checks for empty table, possibility to play power, etc. and query for which power to play
        default:
            return false;
    }
}

template<GridSize gridSize, GameType gameType>
void Game<gridSize, gameType>::run() {
    size_t iterationIndex = 0;
   
    while (checkEndOfGame(iterationIndex % 2 ? m_player1.getColor() : m_player2.getColor())) {
        std::cout << "Player " << (iterationIndex + 1) % 2 + 1 << "'s turn!" << std::endl;

        printBoard();

        if (playerTurn(iterationIndex % 2 ? m_player1.getColor() : m_player2.getColor(), iterationIndex))
            iterationIndex++;

        #ifdef WINDOWS
                std::system("cls");
        #else
                std::system("clear");
        #endif
    }

    printBoard();

    if (m_winner == Card::Color::Undefined) {
        std::cout << "Draw" << std::endl;
        return;
    }

    std::cout << "Winner: " << (m_player1.getColor() == m_winner ? "Player 1" : "Player 2") << std::endl;
}
