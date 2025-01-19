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

    // Constructor public pentru crearea unei noi instanțe Match
    Match(MatchType matchType, TimerDuration timerDuration, Game::GameType gameType, bool illusions, bool explosion)
        : m_matchType(matchType),
        m_timerDuration(timerDuration),
        m_gameType(gameType),
        m_illusions(illusions),
        m_explosion(explosion)
    {
        size_t arenaSize = gameType == Game::GameType::Training ? 3 : 4;
        m_arena.resize(arenaSize);
        for (auto& row : m_arena) {
            row.resize(arenaSize);
        }
    }

    // Dezactivăm copierea
    Match(const Match&) = delete;
    Match& operator=(const Match&) = delete;

    // Metode publice
    size_t checkArenaWin();
    size_t runArenaLogic(GameEndInfo& _information);
    const Matrix<Piece>& getArena() const { return m_arena; }

    nlohmann::json toJson(bool startPlayer, int index, int matchesPlayed, Game& game) {
        nlohmann::json json;

        json["startPlayer"] = startPlayer;
        json["matchIndex"] = index;
        json["totalMatches"] = matchesPlayed;
        json["gameType"] = this->m_gameType;
        json["matchType"] = this->m_matchType;
        json["timerDuration"] = this->m_timerDuration;
        json["illusions"] = this->m_illusions;
        json["explosion"] = this->m_explosion;
        json["p1score"] = this->m_scores.first;
        json["p2score"] = this->m_scores.second;
        json["game"] = game.getJson();
        json["wizardsUsed"] = this->m_wizardsUsed;

        nlohmann::json jsonArray = nlohmann::json::array();
        for (const auto& layer1 : this->m_arena) {
            nlohmann::json layer1Array = nlohmann::json::array();
            for (const auto& layer2 : layer1) {
                nlohmann::json layer2Array = nlohmann::json::array();
                for (const auto& piece : layer2) {
                    layer2Array.push_back(piece.toJson());
                }
                layer1Array.push_back(layer2Array);
            }
            jsonArray.push_back(layer1Array);
        }

        json["arena"] = jsonArray;
        json["index"] = index;
        json["matchesPlayed"] = matchesPlayed;
        json["startPlayer"] = startPlayer;

        return json;
    }

    bool startPlayer{};
    size_t index{ 0 }, matchesPlayed{};

private:
    // Metode private
    std::pair<size_t, size_t> generateWizardIndices(std::mt19937& _gen);
    void printArena() const;
    void calculateArenaWinner();
    void runPrintLogic(size_t _index, size_t _matchesPlayed);
    int runScoreLogic(GameEndInfo& _information, size_t& _matchesPlayed, size_t& _winner, size_t& _winsNeeded);
    void runWinnerLogic(size_t _winner);
    void runMatch(const nlohmann::json& _json = nlohmann::json{});
    void saveJson(bool startPlayer, int index, int matchesPlayed, Game& game);

    static std::string formatScore(float score) {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(1) << score;
        return stream.str();
    }

    // Membri privați
    Matrix<Piece> m_arena{};
    MatchType m_matchType{};
    Game::GameType m_gameType{};
    TimerDuration m_timerDuration{};
    bool m_illusions{};
    bool m_explosion{};
    std::string m_path{};
    std::pair<float, float> m_scores = { 0, 0 };
    std::array<bool, 8> m_wizardsUsed{ false };
};