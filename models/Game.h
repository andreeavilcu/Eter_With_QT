#pragma once

#include <vector>

#include "Player.h"

#ifndef GAME_H
#define GAME_H

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

    enum class ExplosionEffect : size_t {
        None = 0,
        RemoveCard,
        ReturnCard,
        SinkHole,
    };

protected:
    friend class Wizard;
    friend class Power;

    class Board {
        std::vector<std::vector<std::vector<Card>>> m_board{};
        std::pair<size_t, size_t> m_hole{-1, -1};
        std::pair<size_t, size_t> m_minus{ -1, -1 };
        std::pair<size_t, size_t> m_plus{ -1, -1 };

        friend class Game;
        friend class Wizard;
        friend class Power;

        explicit Board(size_t _size);
        [[nodiscard]] size_t getSize() const;

        void circularShiftUp();
        void circularShiftDown();
        void circularShiftLeft();
        void circularShiftRight();

        void printBoard() const;

        [[nodiscard]] bool isAPile(size_t _row, size_t _col) const;
        [[nodiscard]] bool checkIndexes(size_t _row, size_t _col) const;
        [[nodiscard]] bool checkNeighbours(size_t _row, size_t _col) const;
        [[nodiscard]] bool checkValue(size_t _row, size_t _col, Card::Value _value, bool _illusion = false) const;
        [[nodiscard]] bool checkHole(size_t _row, size_t _col) const;

        [[nodiscard]] bool checkIllusion(size_t _row, size_t _col, Card::Color _color) const;
        [[nodiscard]] bool checkIllusionValue(size_t _row, size_t _col, size_t _value) const;

        void resetIllusion(size_t _row, size_t _col);

        [[nodiscard]] Card::Color checkRows() const;
        [[nodiscard]] Card::Color checkCols() const;
        [[nodiscard]] Card::Color checkDiagonals() const;

        [[nodiscard]] bool checkFullBoard() const;
        [[nodiscard]] Card::Color calculateWinner() const;

        [[nodiscard]] bool checkTwoRows() const;
        std::vector<Card> useExplosion(const std::vector<std::vector<Game::ExplosionEffect>>& _matrix);

        [[nodiscard]] bool checkBoardIntegrity() const;

        void placeCard(size_t _row, size_t _col, const Card&& _card);
    };

    Board m_board;
    GameType m_gameType;

    Player m_player1, m_player2;
    Card::Color m_winner{ Card::Color::Undefined };
    std::vector<Card> m_returnedCards{};

    bool m_playedExplosion{ false };

public:
    explicit Game(GameType _gameType);

    void run();

protected:
    void shiftBoard();

    [[nodiscard]] bool checkEmptyDeck() const;
    [[nodiscard]] bool checkCardAfterReturn(Card::Color _color, Card::Value _value) const;

    [[nodiscard]] bool checkEndOfGame(Card::Color _color);

    [[nodiscard]] bool checkPartial(size_t _x, size_t _y, size_t _value, size_t _iterationIndex) const;

    [[nodiscard]] bool playCard(Card::Color _color, size_t _iterationIndex);
    [[nodiscard]] bool playIllusion(Card::Color _color, size_t _iterationIndex);
    [[nodiscard]] bool playWizard(Card::Color _color);
    [[nodiscard]] bool playPower(Card::Color _color);

    void playExplosion();

    [[nodiscard]] std::vector<std::vector<Game::ExplosionEffect>> generateExplosion(size_t _size);
    [[nodiscard]] bool rotateExplosion(std::vector<std::vector<Game::ExplosionEffect>>& _matrix, bool& _quit);
    void rotateMatrixRight(std::vector<std::vector<Game::ExplosionEffect>>& _matrix);
    void printExplosion(const std::vector<std::vector<Game::ExplosionEffect>>& _matrix) const;

    bool playerTurn(Card::Color _color, size_t _iterationIndex);
};

#endif // GAME_H