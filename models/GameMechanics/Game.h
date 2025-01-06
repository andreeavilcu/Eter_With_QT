#pragma once

#include <stack>
#include <vector>
#include <nlohmann/json.hpp>

#include "Board.h"
#include "../Player/Player.h"
#include "GameEndInfo.h"
#include "../Actions/Explosion.h"

inline bool running = true;
inline bool saving;

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

    Board m_board;
    GameType m_gameType;

    Player m_player1, m_player2;
    Card::Color m_winner{ Card::Color::Undefined };

    nlohmann::json m_json;

public:
    std::vector<Card> m_returnedCards{};
    std::vector<Card> m_eliminatedCards{};

    bool m_illusionsAllowed{};
    bool m_explosionAllowed{};

    bool m_playedExplosion{ false };

    explicit Game(GameType _gameType, const std::pair<size_t, size_t>& _wizardIndices, bool _illusions, bool _explosion);
    // explicit Game(const nlohmann::json& _json);

    Board& getBoard() { return m_board; }
    [[nodiscard]] GameType getGameType() const { return m_gameType; }

    Player& getPlayer1() { return m_player1; }
    Player& getPlayer2() { return m_player2; }

    GameEndInfo run(bool _player1Turn, bool _timed, int _duration);

    [[nodiscard]] bool checkEmptyDeck() const;
    [[nodiscard]] std::pair<bool, bool> checkEndOfGame(Card::Color _color);

    nlohmann::json getJson() const { return m_json; }

};

