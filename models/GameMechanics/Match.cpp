#include "Match.h"

std::pair<size_t, size_t> Match::generateWizardIndices(std::mt19937& _gen) {
    std::uniform_int_distribution<size_t> wizardDistribution{ 0, Wizard::wizard_count - 1 };

    std::pair<size_t, size_t> wizardIndices = {-1, -1};

    if (m_gameType == Game::GameType::WizardDuel || m_gameType == Game::GameType::WizardAndPowerDuel) {
        do {
            wizardIndices.first = wizardDistribution(_gen);
        }
        while (this->m_wizardsUsed[wizardIndices.first]);

        this->m_wizardsUsed[wizardIndices.first] = true;

        do {
            wizardIndices.second = wizardDistribution(_gen);
        }
        while (this->m_wizardsUsed[wizardIndices.second]);

        this->m_wizardsUsed[wizardIndices.second] = true;
    }

    return wizardIndices;
}

void Match::printArena() const {
    std::cout << "Arena:\n\n";

    for (const auto& row : this->m_arena) {
        for (const auto& position : row) {
            std::pair<size_t, size_t> pieceCounts{0, 0};

            for (const auto& piece : position) {
                if (piece.getColor() == Card::Color::Red)
                    pieceCounts.first++;

                else pieceCounts.second++;
            }

            std::cout << pieceCounts.first << "R" << pieceCounts.second << "B ";
        }

        std::cout << std::endl;
    }

    std::cout << std::endl;
}

size_t Match::runArenaLogic(GameEndInfo& _information) {
    if (_information.x == -1 || _information.y == -1)
        do {
            std::cout << "Enter piece coordinates: ";
            std::cin >> _information.x >> _information.y;
        } while (_information.x >= this->m_arena.size() || _information.y >= this->m_arena.size());

    auto& pieces = this->m_arena[_information.x][_information.y];
    auto enemyColor = _information.winner == Card::Color::Red ? Card::Color::Blue : Card::Color::Red;

    auto it = std::ranges::find_if(pieces, [enemyColor](const Piece& piece) {
        return piece.getColor() == enemyColor;
    });

    if (it != pieces.end())
        pieces.erase(it);

    pieces.emplace_back(_information.winner);

    return this->checkArenaWin();
}

size_t Match::checkArenaWin() {
    size_t n = m_arena.size();

    for (size_t i = 0; i < n; ++i) {
        int p1 = 0, p2 = 0;
        for (size_t j = 0; j < n; ++j) {
            bool v1 = false, v2 = false;

            for (const auto& piece : this->m_arena[i][j]) {
                if (piece.getColor() == Card::Color::Red) {
                    if (!v1) {
                        v1 = true;
                        p1++;
                    }
                }
                else if (piece.getColor() == Card::Color::Blue) {
                    if (!v2) {
                        v2 = true;
                        p2++;
                    }
                }
            }
        }

        if (p1 >= arenaWinLength) return 1;
        if (p2 >= arenaWinLength) return 2;
    }

    for (size_t i = 0; i < n; ++i) {
        int p1 = 0, p2 = 0;
        for (size_t j = 0; j < n; ++j) {
            bool v1 = false, v2 = false;

            for (const auto& piece : this->m_arena[j][i]) {
                if (piece.getColor() == Card::Color::Red) {
                    if (!v1) {
                        v1 = true;
                        p1++;
                    }
                }
                else if (piece.getColor() == Card::Color::Blue) {
                    if (!v2) {
                        v2 = true;
                        p2++;
                    }
                }
            }
        }

        if (p1 >= arenaWinLength) return 1;
        if (p2 >= arenaWinLength) return 2;
    }

    int p1 = 0, p2 = 0;
    for (size_t i = 0; i < n; ++i) {
        bool v1 = false, v2 = false;
        for (const auto& piece : this->m_arena[i][i]) {
            if (piece.getColor() == Card::Color::Red) {
                if (!v1) {
                    v1 = true;
                    p1++;
                }
            }
            else if (piece.getColor() == Card::Color::Blue) {
                if (!v2) {
                    v2 = true;
                    p2++;
                }
            }
        }

        if (p1 >= arenaWinLength) return 1;
        if (p2 >= arenaWinLength) return 2;
    }

    p1 = 0, p2 = 0;
    for (size_t i = 0; i < n; ++i) {
        bool v1 = false, v2 = false;
        for (const auto& piece : this->m_arena[i][n - i - 1]) {
            if (piece.getColor() == Card::Color::Red) {
                if (!v1) {
                    v1 = true;
                    p1++;
                }
            }
            else if (piece.getColor() == Card::Color::Blue) {
                if (!v2) {
                    v2 = true;
                    p2++;
                }
            }
        }

        if (p1 >= arenaWinLength) return 1;
        if (p2 >= arenaWinLength) return 2;
    }

    return 0;
}

void Match::calculateArenaWinner() {
    for (const auto& row : this->m_arena)
        for (const auto& position : row)
            for (const auto& piece : position)
                if (piece.getColor() == Card::Color::Red) ++this->m_scores.first;
                else if (piece.getColor() == Card::Color::Blue) ++this->m_scores.second;
}

void Match::runPrintLogic(size_t _index, size_t _matchesPlayed) {
    if (this->m_matchType == MatchType::Tournament) this->printArena();

    else {
        std::cout << "Match Score: Player1 - Player2\n";
        std::cout << "Game " + std::to_string(_index) + "/" + std::to_string(_matchesPlayed) +
            "        " + formatScore(this->m_scores.first) +
            "  -  " + formatScore(this->m_scores.second);
        std::cout << std::endl << std::endl;
    }
}

int Match::runScoreLogic(GameEndInfo &_information, size_t &_matchesPlayed, size_t &_winner, size_t &_winsNeeded) {
    if (this->m_matchType == MatchType::Tournament) {
        if (_information.winner == Card::Color::Undefined)
            return 2;

        _winner = this->runArenaLogic(_information);

        if (_winner) return 1;
    }

    else {
        switch (_information.winner) {
            case Card::Color::Red:
                this->m_scores.first++;
            break;
            case Card::Color::Blue:
                this->m_scores.second++;
            break;
            case Card::Color::Undefined:
                this->m_scores.first += 0.5f;
            this->m_scores.second += 0.5f;

            _matchesPlayed++;
        }

        if (this->m_scores.first >= static_cast<float>(_winsNeeded) || this->m_scores.second >= static_cast<float>(_winsNeeded))
            return 1;
    }

    return 0;
}

void Match::runWinnerLogic(size_t _winner) {
    if (_winner) {
        std::cout << "Match winner: " << (_winner == 1? "Red" : "Blue") << " player\n";
        return;
    }

    if (this->m_matchType == MatchType::Tournament) {
        this->calculateArenaWinner();
    }

    if (this->m_scores.first == this->m_scores.second) {
        std::cout << "Match ended in a draw." << std::endl;
        return;
    }

    _winner = this->m_scores.first > this->m_scores.second ? 1 : 2;

    std::cout << "Match winner: " << (_winner == 1? "Red" : "Blue") << " player\n";
}

void Match::runMatch() {
    std::random_device rd;
    std::mt19937 gen{ rd() };
    std::bernoulli_distribution startPlayerDistribution{ 0.5 };

    size_t winner = 0;
    const bool startPlayer = startPlayerDistribution(gen);
    const bool timed = this->m_timerDuration != TimerDuration::Untimed;

    size_t matchesPlayed = m_gameType == Game::GameType::Training && m_matchType != MatchType::Tournament ? 3 : 5;
    size_t winsNeeded = matchesPlayed / 2 + 1;

    for (size_t index = 0; index < matchesPlayed; index++) {
        std::pair<size_t, size_t> wizardIndices = generateWizardIndices(gen);

        runPrintLogic(index, matchesPlayed);

        Game game{ m_gameType , wizardIndices, this->m_illusions, this->m_explosion, this->m_matchType == MatchType::Tournament};
        GameEndInfo information = game.run(index % 2 == startPlayer, timed, static_cast<int>(this->m_timerDuration));

        if (!running) {
            if (saving) saveJson(startPlayer, index, matchesPlayed, game);
            return;
        }

        auto id = runScoreLogic(information, matchesPlayed, winner, winsNeeded);

        if (id == 2) continue;
        if (id == 1) break;
    }

    runWinnerLogic(winner);
}

void Match::runMatch(const nlohmann::json &_json) {
    size_t winner = 0;

    bool timed = static_cast<bool>(_json["timerDuration"].get<float>());
    bool startPlayer = _json["startPlayer"].get<bool>();

    size_t matchesPlayed = _json["totalMatches"].get<int>();

    size_t winsNeeded = matchesPlayed / 2 + 1;

    for (size_t index = _json["matchIndex"].get<int>(); index < matchesPlayed; index++) {
        runPrintLogic(index, matchesPlayed);

        auto j = _json["game"];

        Game game{ m_gameType, _json["game"] , this->m_illusions, this->m_explosion, this->m_matchType == MatchType::Tournament };
        GameEndInfo information = game.run(_json["game"], timed, static_cast<int>(this->m_timerDuration));

        if (!running) {
            if (saving) saveJson(startPlayer, index, matchesPlayed, game);
            return;
        }

        auto id = runScoreLogic(information, matchesPlayed, winner, winsNeeded);

        if (id == 2) continue;
        if (id == 1) break;
    }


}

void Match::saveJson(bool startPlayer, int index, int matchesPlayed, Game& game) {
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

    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    std::string dir = "SavedGames/";

    std::filesystem::create_directories(dir);

    std::ostringstream timestamp;
    timestamp << std::put_time(std::localtime(&now_time), "%Y-%m-%d_%H-%M-%S");

    std::string filename = dir + "backup_" + timestamp.str() + ".json";

    std::ofstream file(filename);

    file << json;
    file.close();
}
