#pragma once

#include "Game.h"

class Match {
public:
    enum class MatchType : size_t {
        Normal = 0,
        Tournament,
        Timed,
        TimedTournament,
    };

    Match(const Match&) = delete;
    Match& operator=(const Match&) = delete;

    static Match& getInstance(MatchType _matchType, Game::GameType _gameType, bool _illusions, bool _explosion) {
        static Match instance;

        instance.m_matchType = _matchType;
        instance.m_gameType = _gameType;

        instance.m_illusions = _illusions;
        instance.m_explosion = _explosion;

        return instance;
    }


private:
    Match() = default;
    ~Match() = default;

    MatchType m_matchType{};
    Game::GameType m_gameType{};

    bool m_illusions{};
    bool m_explosion{};

    std::pair<size_t, size_t> m_scores = {0, 0};
    std::array<bool, 8> m_wizardsUsed{ false };

public:
    void runMatch();

};
