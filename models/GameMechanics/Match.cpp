#include "Match.h"

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

void Match::runMatch() {
    std::random_device rd;
    std::mt19937 gen{ rd() };
    std::bernoulli_distribution startPlayerDistribution{ 0.5 };
    std::uniform_int_distribution<size_t> wizardDistribution{ 0, Wizard::wizard_count - 1 };

    size_t winner = 0;
    const bool startPlayer = startPlayerDistribution(gen);
    const bool timed = this->m_timer_duration != TimerDuration::Untimed;

    size_t matchesPlayed = m_gameType == Game::GameType::Training && m_matchType != MatchType::Tournament ? 3 : 5;
    size_t winsNeeded = matchesPlayed / 2 + 1;

    if (this->m_matchType == MatchType::Tournament) {
        size_t arenaSize = m_gameType == Game::GameType::Training ? 3 : 4;

        this->m_arena.resize(arenaSize);

        for (auto& row : this->m_arena)
            row.resize(arenaSize);
    }

    for (size_t index = 0; index < matchesPlayed; index++) {
        std::pair<size_t, size_t> wizardIndices = {-1, -1};

        if (m_gameType == Game::GameType::WizardDuel || m_gameType == Game::GameType::WizardAndPowerDuel) {
            do {
                wizardIndices.first = wizardDistribution(gen);
            }
            while (this->m_wizardsUsed[wizardIndices.first]);

            this->m_wizardsUsed[wizardIndices.first] = true;

            do {
                wizardIndices.second = wizardDistribution(gen);
            }
            while (this->m_wizardsUsed[wizardIndices.second]);

            this->m_wizardsUsed[wizardIndices.second] = true;
        }

        if (this->m_matchType == MatchType::Tournament) this->printArena();

        else {
            std::cout << "Match Score: Player1 - Player2\n";
            std::cout << "Game " + std::to_string(index) + "/" + std::to_string(matchesPlayed) +
                "        " + formatScore(this->m_scores.first) +
                "  -  " + formatScore(this->m_scores.second);
            std::cout << std::endl << std::endl;
        }

        Game game{ m_gameType , wizardIndices, this->m_illusions, this->m_explosion};
        GameEndInfo information = game.run(index % 2 == startPlayer, timed, static_cast<int>(this->m_timer_duration));

        if (this->m_matchType == MatchType::Tournament) {
            if (information.winner == Card::Color::Undefined)
                continue;

            winner = this->runArenaLogic(information);

            if (winner) break;
        }

        else {
            switch (information.winner) {
                case Card::Color::Red:
                    this->m_scores.first++;
                break;
                case Card::Color::Blue:
                    this->m_scores.second++;
                break;
                case Card::Color::Undefined:
                    this->m_scores.first += 0.5f;
                this->m_scores.second += 0.5f;

                matchesPlayed++;
            }

            if (this->m_scores.first >= static_cast<float>(winsNeeded) || this->m_scores.second >= static_cast<float>(winsNeeded))
                break;
        }
    }

    if (winner) {
        std::cout << "Match winner: " << (winner == 1? "Red" : "Blue") << " player\n";
        return;
    }

    if (this->m_matchType == MatchType::Tournament) {
        this->calculateArenaWinner();
    }

    if (this->m_scores.first == this->m_scores.second) {
        std::cout << "Match ended in a draw." << std::endl;
        return;
    }

    winner = this->m_scores.first > this->m_scores.second ? 1 : 2;

    std::cout << "Match winner: " << (winner == 1? "Red" : "Blue") << " player\n";
}
