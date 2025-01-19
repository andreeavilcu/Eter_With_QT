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

        instance.m_timerDuration = _timer_duration;

        instance.m_illusions = _illusions;
        instance.m_explosion = _explosion;

        if (instance.m_matchType != MatchType::Tournament) return;

        size_t arenaSize = instance.m_gameType == Game::GameType::Training ? 3 : 4;

        instance.m_arena.resize(arenaSize);

        for (auto& row : instance.m_arena)
            row.resize(arenaSize);

        instance.runMatch();
    }

    static void run(const std::string& _path) {
        static Match instance;

        instance.m_path = _path;

        std::ifstream configFile(_path);

        if (!configFile.is_open()) {
            std::cerr << "Failed to open config file" << std::endl;
            return;
        }

        nlohmann::json json;

        try {
            configFile >> json;
        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "Parse error: " << e.what() << std::endl;
            return;
        }

        instance.m_matchType = json["matchType"].get<MatchType>();
        instance.m_gameType = json["gameType"].get<Game::GameType>();

        instance.m_timerDuration = json["timerDuration"].get<TimerDuration>();

        instance.m_illusions = json["illusions"].get<bool>();
        instance.m_explosion = json["explosion"].get<bool>();

        instance.m_scores.first = json["p1score"].get<float>();
        instance.m_scores.second = json["p2score"].get<float>();

        instance.m_wizardsUsed = json["wizardsUsed"].get<std::array<bool, 8>>();

        instance.index = json["index"].get<size_t>();

        for (const auto& layer1Array : json["arena"]) {
            std::vector<std::deque<Piece>> layer1;
            for (const auto& layer2Array : layer1Array) {
                std::deque<Piece> layer2;
                for (const auto& piece : layer2Array) {
                    layer2.emplace_back(piece);
                }
                layer1.push_back(layer2);
            }

            instance.m_arena.push_back(layer1);
        }

        instance.runMatch(json);
    }

    bool startPlayer{};
    size_t index{ 0 }, matchesPlayed{};

private:
    Match() = default;
    ~Match() = default;

    Matrix<Piece> m_arena{};

    MatchType m_matchType{};
    Game::GameType m_gameType{};

    TimerDuration m_timerDuration{};

    bool m_illusions{};
    bool m_explosion{};

    std::string m_path{};

    std::pair<float, float> m_scores = {0, 0};
    std::array<bool, 8> m_wizardsUsed{ false };

    static std::string formatScore(float score) {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(1) << score;
        return stream.str();
    }

    std::pair<size_t, size_t> generateWizardIndices(std::mt19937& _gen);

    void printArena() const;
    size_t checkArenaWin();
    void calculateArenaWinner();
    size_t runArenaLogic(GameEndInfo& _information);

    void runPrintLogic(size_t _index, size_t _matchesPlayed);
    int runScoreLogic(GameEndInfo& _information, size_t& _matchesPlayed, size_t& _winner, size_t& _winsNeeded);
    void runWinnerLogic(size_t _winner);

    void runMatch(const nlohmann::json& _json = nlohmann::json{});

    void saveJson(bool startPlayer, int index, int matchesPlayed, Game& game);

public:
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
};
