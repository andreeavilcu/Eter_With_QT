#pragma once

#include <vector>

#include "Player.h"

class Game {
public:
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

protected:
    friend class Wizard;
    friend class Power;

    class Board {
        std::vector<std::vector<std::vector<Card>>> m_board{};

    public:
        explicit Board(size_t _size);

        void circularShiftUp();
        void circularShiftDown();
        void circularShiftLeft();
        void circularShiftRight();

        void printBoard() const;

        [[nodiscard]] bool checkIndexes(size_t _row, size_t _col) const;
        [[nodiscard]] bool checkNeighbours(size_t _row, size_t _col) const;
        [[nodiscard]] bool checkValue(size_t _row, size_t _col, const Card::Value& _value, bool _illusion = false) const;

        [[nodiscard]] bool checkIllusion(size_t _row, size_t _col, Card::Color _color) const;
        [[nodiscard]] bool checkIllusionValue(size_t _row, size_t _col, size_t _value) const;

        void resetIllusion(size_t _row, size_t _col);

        [[nodiscard]] Card::Color checkRows() const;
        [[nodiscard]] Card::Color checkCols() const;
        [[nodiscard]] Card::Color checkDiagonals() const;

        [[nodiscard]] bool checkFullBoard() const;
        [[nodiscard]] Card::Color calculateWinner() const;

        [[nodiscard]] bool checkTwoRows() const;

        void placeCard(size_t _row, size_t _col, const Card&& _card);
    };

    Board m_board;

    Player m_player1, m_player2;
    Card::Color m_winner{ Card::Color::Undefined };

    bool m_playedExplosion{ false };

public:
    explicit Game(GameType _gameType);

    void run();

protected:
    void shiftBoard();

    [[nodiscard]] bool checkEmptyDeck() const;

    [[nodiscard]] bool checkEndOfGame(Card::Color _color);

    bool playCard(Card::Color _color, size_t _iterationIndex);
    bool playIllusion(Card::Color _color, size_t _iterationIndex);
    void playExplosion();

    bool playerTurn(Card::Color _color, size_t _iterationIndex);
};

