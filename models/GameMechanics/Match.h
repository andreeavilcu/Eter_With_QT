#pragma once

#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <chrono>
#include <nlohmann/json.hpp>

#include "Game.h"
#include "../Actions/Piece.h"

class Match {
public:
    enum class MatchType : size_t {
        Normal = 0,
        Tournament,
    };

    enum class TimerDuration : size_t {
        Untimed = 0,
        Short = 60,
        Medium = 90,
        Long = 120,
    };

    static constexpr auto arenaWinLength = 3;

    Match(const Match&) = delete;
    Match& operator=(const Match&) = delete;

    static void run(const MatchType _matchType, const TimerDuration _timer_duration, Game::GameType _gameType, const bool _illusions, const bool _explosion) {
        static Match instance;

        instance.m_matchType = _matchType;
        instance.m_gameType = _gameType;

        instance.m_timer_duration = _timer_duration;

        instance.m_illusions = _illusions;
        instance.m_explosion = _explosion;

        if (instance.m_matchType != MatchType::Tournament) return;

        size_t arenaSize = instance.m_gameType == Game::GameType::Training ? 3 : 4;

        instance.m_arena.resize(arenaSize);

        for (auto& row : instance.m_arena)
            row.resize(arenaSize);

        instance.runMatch();
    }

    static void run(const nlohmann::json& _json) {
        static Match instance;

        instance.m_matchType = _json["matchType"].get<MatchType>();
        instance.m_gameType = _json["gameType"].get<Game::GameType>();

        instance.m_timer_duration = _json["timerDuration"].get<TimerDuration>();

        instance.m_illusions = _json["illusions"].get<bool>();
        instance.m_explosion = _json["explosion"].get<bool>();

        instance.m_scores.first = _json["p1score"].get<float>();
        instance.m_scores.second = _json["p2score"].get<float>();

        instance.m_wizardsUsed = _json["wizardsUsed"].get<std::array<bool, 8>>();

        for (const auto& layer1Array : _json["arena"]) {
            std::vector<std::vector<Piece>> layer1;
            for (const auto& layer2Array : layer1Array) {
                std::vector<Piece> layer2;
                for (const auto& piece : layer2Array) {
                    layer2.emplace_back(piece);
                }
                layer1.push_back(layer2);
            }

            instance.m_arena.push_back(layer1);
        }

        instance.runMatch(_json);

    }


private:
    Match() = default;
    ~Match() = default;

    Matrix<Piece> m_arena{};

    MatchType m_matchType{};
    Game::GameType m_gameType{};

    TimerDuration m_timer_duration{};

    bool m_illusions{};
    bool m_explosion{};

    std::pair<float, float> m_scores = {0, 0};
    std::array<bool, 8> m_wizardsUsed{ false };

    static std::string formatScore(float score) {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(1) << score;
        return stream.str();
    }

    std::pair<size_t, size_t> generateWizardIndices(std::mt19937& _gen);

    void printArena() const;
    size_t runArenaLogic(GameEndInfo& _information);
    size_t checkArenaWin();
    void calculateArenaWinner();

    void runPrintLogic(size_t _index, size_t _matchesPlayed);
    int runScoreLogic(GameEndInfo& _information, size_t& _matchesPlayed, size_t& _winner, size_t& _winsNeeded);
    void runWinnerLogic(size_t _winner);

    void runMatch();
    void runMatch(const nlohmann::json& _json);

    void saveJson(bool startPlayer, int index, int matchesPlayed, Game& game);
};
