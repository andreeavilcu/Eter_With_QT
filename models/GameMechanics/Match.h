#pragma once

#include <sstream>
#include <iomanip>
#include <string>

#include "Game.h"
#include "../Actions/Piece.h"

class Match {
public:
    enum class MatchType : size_t {
        Normal = 0,
        Tournament,
    };

    static constexpr auto arenaWinLength = 3;

    Match(const Match&) = delete;
    Match& operator=(const Match&) = delete;

    static Match& getInstance(MatchType _matchType, const bool _timed, Game::GameType _gameType, bool _illusions, bool _explosion) {
        static Match instance;

        instance.m_matchType = _matchType;
        instance.m_gameType = _gameType;

        instance.m_timed = _timed;

        instance.m_illusions = _illusions;
        instance.m_explosion = _explosion;

        return instance;
    }


private:
    Match() = default;
    ~Match() = default;

    Matrix<Piece> m_arena{};

    MatchType m_matchType{};
    Game::GameType m_gameType{};

    bool m_timed{};

    bool m_illusions{};
    bool m_explosion{};

    std::pair<float, float> m_scores = {0, 0};
    std::array<bool, 8> m_wizardsUsed{ false };

    static std::string formatScore(float score) {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(1) << score;
        return stream.str();
    }

    void printArena() const;
    size_t runArenaLogic(GameEndInfo& _information);
    size_t checkArenaWin();
    void calculateArenaWinner();

public:
    void runMatch();

};
