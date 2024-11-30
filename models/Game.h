#pragma once

#include <vector>
#include "Player.h"
#include "Board.h" 

#ifndef GAME_H
#define GAME_H

class Board;

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

#endif 
