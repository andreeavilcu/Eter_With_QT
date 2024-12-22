#pragma once

#include <vector>
#include <stack>

#include "IGame.h"
#include "Player.h"
#include "Board.h"
#include "Explosion.h"

#ifndef GAME_H
#define GAME_H

class Game : public IGame{
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

    Board m_board; 
    GameType m_gameType;

    Player m_player1, m_player2;
    Card::Color m_winner{ Card::Color::Undefined };

public:
    std::vector<Card> m_returnedCards{};
    std::vector<Card> m_eliminatedCards{};

    bool m_illusionsAllowed{};
    bool m_explosionAllowed{};

    bool m_playedExplosion{ false };

    explicit Game(GameType _gameType, const std::pair<size_t, size_t>& _wizardIndices, bool _illusions, bool _explosion);

    Board& getBoard() { return m_board; }
    [[nodiscard]] GameType getGameType() const { return m_gameType; }

    Player& getPlayer1() { return m_player1; }
    Player& getPlayer2() { return m_player2; }

    size_t run();

    [[nodiscard]] bool checkEmptyDeck() const;
    [[nodiscard]] bool checkEndOfGame(Card::Color _color);

};

#endif 